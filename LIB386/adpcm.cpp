/*
* ADPCM codecs
* Copyright (c) 2001-2003 The ffmpeg Project
*
* This file is part of FFmpeg.
*
* FFmpeg is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* FFmpeg is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with FFmpeg; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

// yaz: modified to be a single file with no header

#define BLKSIZE 1024

#define CLAMP_TO_SHORT(value) \
if (value > 32767) \
    value = 32767; \
else if (value < -32768) \
    value = -32768; \

/* step_table[] and index_table[] are from the ADPCM reference source */
/* This is the index table: */
static const int index_table[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

/** 
 * This is the step table. Note that many programs use slight deviations from
 * this table, but such deviations are negligible:
 */
static const int step_table[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

/* These are for MS-ADPCM */
/* AdaptationTable[], AdaptCoeff1[], and AdaptCoeff2[] are from libsndfile */
static const int AdaptationTable[] = {
        230, 230, 230, 230, 307, 409, 512, 614,
        768, 614, 512, 409, 307, 230, 230, 230
};

static const int AdaptCoeff1[] = {
        256, 512, 0, 192, 240, 460, 392
};

static const int AdaptCoeff2[] = {
        0, -256, 0, 64, 0, -208, -232
};

/* These are for CD-ROM XA ADPCM */
static const int xa_adpcm_table[5][2] = {
   {   0,   0 },
   {  60,   0 },
   { 115, -52 },
   {  98, -55 },
   { 122, -60 }
};

static int ea_adpcm_table[] = {
    0, 240, 460, 392, 0, 0, -208, -220, 0, 1,
    3, 4, 7, 8, 10, 11, 0, -1, -3, -4
};

static int ct_adpcm_table[8] = {
    0x00E6, 0x00E6, 0x00E6, 0x00E6,
    0x0133, 0x0199, 0x0200, 0x0266
};

/* end of tables */

typedef struct ADPCMChannelStatus {
    int predictor;
    short int step_index;
    int step;
    /* for encoding */
    int prev_sample;

    /* MS version */
    short sample1;
    short sample2;
    int coeff1;
    int coeff2;
    int idelta;
} ADPCMChannelStatus;

typedef struct ADPCMContext {
    int channel; /* for stereo MOVs, decode left, then decode right, then tell it's decoded */
    ADPCMChannelStatus status[2];
    short sample_buffer[32]; /* hold left samples while waiting for right samples */
} ADPCMContext;

/* XXX: implement encoding */

#ifdef CONFIG_ENCODERS
static int adpcm_encode_init(AVCodecContext *avctx)
{
    if (avctx->channels > 2)
        return -1; /* only stereo or mono =) */
    switch(avctx->codec->id) {
    case CODEC_ID_ADPCM_IMA_QT:
        av_log(avctx, AV_LOG_ERROR, "ADPCM: codec adpcm_ima_qt unsupported for encoding !\n");
        avctx->frame_size = 64; /* XXX: can multiple of avctx->channels * 64 (left and right blocks are interleaved) */
        return -1;
        break;
    case CODEC_ID_ADPCM_IMA_WAV:
        avctx->frame_size = (BLKSIZE - 4 * avctx->channels) * 8 / (4 * avctx->channels) + 1; /* each 16 bits sample gives one nibble */
                                                             /* and we have 4 bytes per channel overhead */
        avctx->block_align = BLKSIZE;
        /* seems frame_size isn't taken into account... have to buffer the samples :-( */
        break;
    case CODEC_ID_ADPCM_MS:
        avctx->frame_size = (BLKSIZE - 7 * avctx->channels) * 2 / avctx->channels + 2; /* each 16 bits sample gives one nibble */
                                                             /* and we have 7 bytes per channel overhead */
        avctx->block_align = BLKSIZE;
        break;
    default:
        return -1;
        break;
    }

    avctx->coded_frame= avcodec_alloc_frame();
    avctx->coded_frame->key_frame= 1;

    return 0;
}

static int adpcm_encode_close(AVCodecContext *avctx)
{
    av_freep(&avctx->coded_frame);

    return 0;
}


static inline unsigned char adpcm_ima_compress_sample(ADPCMChannelStatus *c, short sample)
{
    int step_index;
    unsigned char nibble;
    
    int sign = 0; /* sign bit of the nibble (MSB) */
    int delta, predicted_delta;

    delta = sample - c->prev_sample;

    if (delta < 0) {
        sign = 1;
        delta = -delta;
    }

    step_index = c->step_index;

    /* nibble = 4 * delta / step_table[step_index]; */
    nibble = (delta << 2) / step_table[step_index];

    if (nibble > 7)
        nibble = 7;

    step_index += index_table[nibble];
    if (step_index < 0)
        step_index = 0;
    if (step_index > 88)
        step_index = 88;

    /* what the decoder will find */
    predicted_delta = ((step_table[step_index] * nibble) / 4) + (step_table[step_index] / 8);

    if (sign)
        c->prev_sample -= predicted_delta;
    else
        c->prev_sample += predicted_delta;

    CLAMP_TO_SHORT(c->prev_sample);


    nibble += sign << 3; /* sign * 8 */   

    /* save back */
    c->step_index = step_index;

    return nibble;
}

static inline unsigned char adpcm_ms_compress_sample(ADPCMChannelStatus *c, short sample)
{
    int predictor, nibble, bias;

    predictor = (((c->sample1) * (c->coeff1)) + ((c->sample2) * (c->coeff2))) / 256;
    
    nibble= sample - predictor;
    if(nibble>=0) bias= c->idelta/2;
    else          bias=-c->idelta/2;
        
    nibble= (nibble + bias) / c->idelta;
    nibble= clip(nibble, -8, 7)&0x0F;
    
    predictor += (signed)((nibble & 0x08)?(nibble - 0x10):(nibble)) * c->idelta;
    CLAMP_TO_SHORT(predictor);

    c->sample2 = c->sample1;
    c->sample1 = predictor;

    c->idelta = (AdaptationTable[(int)nibble] * c->idelta) >> 8;
    if (c->idelta < 16) c->idelta = 16;

    return nibble;
}

static int adpcm_encode_frame(AVCodecContext *avctx,
			    unsigned char *frame, int buf_size, void *data)
{
    int n, i, st;
    short *samples;
    unsigned char *dst;
    ADPCMContext *c = avctx->priv_data;

    dst = frame;
    samples = (short *)data;
    st= avctx->channels == 2;
/*    n = (BLKSIZE - 4 * avctx->channels) / (2 * 8 * avctx->channels); */

    switch(avctx->codec->id) {
    case CODEC_ID_ADPCM_IMA_QT: /* XXX: can't test until we get .mov writer */
        break;
    case CODEC_ID_ADPCM_IMA_WAV:
        n = avctx->frame_size / 8;
            c->status[0].prev_sample = (signed short)samples[0]; /* XXX */
/*            c->status[0].step_index = 0; *//* XXX: not sure how to init the state machine */
            *dst++ = (c->status[0].prev_sample) & 0xFF; /* little endian */
            *dst++ = (c->status[0].prev_sample >> 8) & 0xFF;
            *dst++ = (unsigned char)c->status[0].step_index;
            *dst++ = 0; /* unknown */
            samples++;
            if (avctx->channels == 2) {
                c->status[1].prev_sample = (signed short)samples[1];
/*                c->status[1].step_index = 0; */
                *dst++ = (c->status[1].prev_sample) & 0xFF;
                *dst++ = (c->status[1].prev_sample >> 8) & 0xFF;
                *dst++ = (unsigned char)c->status[1].step_index;
                *dst++ = 0;
                samples++;
            }
        
            /* stereo: 4 bytes (8 samples) for left, 4 bytes for right, 4 bytes left, ... */
            for (; n>0; n--) {
                *dst = adpcm_ima_compress_sample(&c->status[0], samples[0]) & 0x0F;
                *dst |= (adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels]) << 4) & 0xF0;
                dst++;
                *dst = adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 2]) & 0x0F;
                *dst |= (adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 3]) << 4) & 0xF0;
                dst++;
                *dst = adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 4]) & 0x0F;
                *dst |= (adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 5]) << 4) & 0xF0;
                dst++;
                *dst = adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 6]) & 0x0F;
                *dst |= (adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 7]) << 4) & 0xF0;
                dst++;
                /* right channel */
                if (avctx->channels == 2) {
                    *dst = adpcm_ima_compress_sample(&c->status[1], samples[1]);
                    *dst |= adpcm_ima_compress_sample(&c->status[1], samples[3]) << 4;
                    dst++;
                    *dst = adpcm_ima_compress_sample(&c->status[1], samples[5]);
                    *dst |= adpcm_ima_compress_sample(&c->status[1], samples[7]) << 4;
                    dst++;
                    *dst = adpcm_ima_compress_sample(&c->status[1], samples[9]);
                    *dst |= adpcm_ima_compress_sample(&c->status[1], samples[11]) << 4;
                    dst++;
                    *dst = adpcm_ima_compress_sample(&c->status[1], samples[13]);
                    *dst |= adpcm_ima_compress_sample(&c->status[1], samples[15]) << 4;
                    dst++;
                }
                samples += 8 * avctx->channels;
            }
        break;
    case CODEC_ID_ADPCM_MS:
        for(i=0; i<avctx->channels; i++){
            int predictor=0;

            *dst++ = predictor;
            c->status[i].coeff1 = AdaptCoeff1[predictor];
            c->status[i].coeff2 = AdaptCoeff2[predictor];
        }
        for(i=0; i<avctx->channels; i++){
            if (c->status[i].idelta < 16) 
                c->status[i].idelta = 16;
            
            *dst++ = c->status[i].idelta & 0xFF;
            *dst++ = c->status[i].idelta >> 8;
        }
        for(i=0; i<avctx->channels; i++){
            c->status[i].sample1= *samples++;

            *dst++ = c->status[i].sample1 & 0xFF;
            *dst++ = c->status[i].sample1 >> 8;
        }
        for(i=0; i<avctx->channels; i++){
            c->status[i].sample2= *samples++;

            *dst++ = c->status[i].sample2 & 0xFF;
            *dst++ = c->status[i].sample2 >> 8;
        }

        for(i=7*avctx->channels; i<avctx->block_align; i++) {
            int nibble;
            nibble = adpcm_ms_compress_sample(&c->status[ 0], *samples++)<<4;
            nibble|= adpcm_ms_compress_sample(&c->status[st], *samples++);
            *dst++ = nibble;
        }
        break;
    default:
        return -1;
    }
    return dst - frame;
}
#endif //CONFIG_ENCODERS

struct AVCodecContext
{
	int block_align;
	int channels;
	ADPCMContext priv_data;
};

AVCodecContext g_AVCodecContext;

int adpcm_decode_init(int numChannels)
{
	g_AVCodecContext.block_align = 0;
	g_AVCodecContext.channels = numChannels;
    ADPCMContext *c = &g_AVCodecContext.priv_data;

    c->channel = 0;
    c->status[0].predictor = c->status[1].predictor = 0;
    c->status[0].step_index = c->status[1].step_index = 0;
    c->status[0].step = c->status[1].step = 0;

    /*switch(avctx->codec->id) {
    case CODEC_ID_ADPCM_CT:
	c->status[0].step = c->status[1].step = 511;
	break;
    default:
        break;
    }*/
    return 0;
}

static inline short adpcm_ima_expand_nibble(ADPCMChannelStatus *c, char nibble, int shift)
{
    int step_index;
    int predictor;
    int sign, delta, diff, step;

    step = step_table[c->step_index];
    step_index = c->step_index + index_table[(unsigned)nibble];
    if (step_index < 0) step_index = 0;
    else if (step_index > 88) step_index = 88;

    sign = nibble & 8;
    delta = nibble & 7;
    /* perform direct multiplication instead of series of jumps proposed by
     * the reference ADPCM implementation since modern CPUs can do the mults
     * quickly enough */
    diff = ((2 * delta + 1) * step) >> shift;
    predictor = c->predictor;
    if (sign) predictor -= diff;
    else predictor += diff;

    CLAMP_TO_SHORT(predictor);
    c->predictor = predictor;
    c->step_index = step_index;

    return (short)predictor;
}

static inline short adpcm_ms_expand_nibble(ADPCMChannelStatus *c, char nibble)
{
    int predictor;

    predictor = (((c->sample1) * (c->coeff1)) + ((c->sample2) * (c->coeff2))) / 256;
    predictor += (signed)((nibble & 0x08)?(nibble - 0x10):(nibble)) * c->idelta;
    CLAMP_TO_SHORT(predictor);

    c->sample2 = c->sample1;
    c->sample1 = predictor;
    c->idelta = (AdaptationTable[(int)nibble] * c->idelta) >> 8;
    if (c->idelta < 16) c->idelta = 16;

    return (short)predictor;
}

static inline short adpcm_ct_expand_nibble(ADPCMChannelStatus *c, char nibble)
{
    int predictor;
    int sign, delta, diff;
    int new_step;

    sign = nibble & 8;
    delta = nibble & 7;
    /* perform direct multiplication instead of series of jumps proposed by
     * the reference ADPCM implementation since modern CPUs can do the mults
     * quickly enough */
    diff = ((2 * delta + 1) * c->step) >> 3;
    predictor = c->predictor;
    /* predictor update is not so trivial: predictor is multiplied on 254/256 before updating */
    if(sign)
	predictor = ((predictor * 254) >> 8) - diff;
    else
    	predictor = ((predictor * 254) >> 8) + diff;
    /* calculate new step and clamp it to range 511..32767 */
    new_step = (ct_adpcm_table[nibble & 7] * c->step) >> 8;
    c->step = new_step;
    if(c->step < 511)
	c->step = 511;
    if(c->step > 32767)
	c->step = 32767;

    CLAMP_TO_SHORT(predictor);
    c->predictor = predictor;
    return (short)predictor;
}

static void xa_decode(short *out, const unsigned char *in, 
    ADPCMChannelStatus *left, ADPCMChannelStatus *right, int inc)
{
    int i, j;
    int shift,filter,f0,f1;
    int s_1,s_2;
    int d,s,t;

    for(i=0;i<4;i++) {

        shift  = 12 - (in[4+i*2] & 15);
        filter = in[4+i*2] >> 4;
        f0 = xa_adpcm_table[filter][0];
        f1 = xa_adpcm_table[filter][1];

        s_1 = left->sample1;
        s_2 = left->sample2;

        for(j=0;j<28;j++) {
            d = in[16+i+j*4];

            t = (signed char)(d<<4)>>4;
            s = ( t<<shift ) + ((s_1*f0 + s_2*f1+32)>>6);
            CLAMP_TO_SHORT(s);
            *out = s;
            out += inc;
            s_2 = s_1;
            s_1 = s;
        }

        if (inc==2) { /* stereo */
            left->sample1 = s_1;
            left->sample2 = s_2;
            s_1 = right->sample1;
            s_2 = right->sample2;
            out = out + 1 - 28*2;
        }

        shift  = 12 - (in[5+i*2] & 15);
        filter = in[5+i*2] >> 4;

        f0 = xa_adpcm_table[filter][0];
        f1 = xa_adpcm_table[filter][1];

        for(j=0;j<28;j++) {
            d = in[16+i+j*4];

            t = (signed char)d >> 4;
            s = ( t<<shift ) + ((s_1*f0 + s_2*f1+32)>>6);
            CLAMP_TO_SHORT(s);
            *out = s;
            out += inc;
            s_2 = s_1;
            s_1 = s;
        }

        if (inc==2) { /* stereo */
            right->sample1 = s_1;
            right->sample2 = s_2;
            out -= 1;
        } else {
            left->sample1 = s_1;
            left->sample2 = s_2;
        }
    }
}


/* DK3 ADPCM support macro */
#define DK3_GET_NEXT_NIBBLE() \
    if (decode_top_nibble_next) \
    { \
        nibble = (last_byte >> 4) & 0x0F; \
        decode_top_nibble_next = 0; \
    } \
    else \
    { \
        last_byte = *src++; \
        if (src >= buf + buf_size) break; \
        nibble = last_byte & 0x0F; \
        decode_top_nibble_next = 1; \
    }

int clip(int inputVal, int range1, int range2)
{
	return inputVal;
}

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef signed short int int16_t;
typedef unsigned long int uint32_t;
typedef signed long int int32_t;
#define AV_LOG_ERROR 0
void av_log(...)
{

}

int adpcm_decode_frame(
			    void *data, int *data_size,
			    uint8_t *buf, int buf_size)
{
	AVCodecContext *avctx = &g_AVCodecContext;
    ADPCMContext *c = &avctx->priv_data;
    ADPCMChannelStatus *cs;
    int n, m, channel, i;
    int block_predictor[2];
    short *samples;
    uint8_t *src;
    int st; /* stereo */

    /* DK3 ADPCM accounting variables */
    unsigned char last_byte = 0;
    unsigned char nibble;
    int decode_top_nibble_next = 0;
    int diff_channel;

    /* EA ADPCM state variables */
    uint32_t samples_in_chunk;
    int32_t previous_left_sample, previous_right_sample;
    int32_t current_left_sample, current_right_sample;
    int32_t next_left_sample, next_right_sample;
    int32_t coeff1l, coeff2l, coeff1r, coeff2r;
    uint8_t shift_left, shift_right;
    int count1, count2;

    if (!buf_size)
        return 0;

    samples = (short*)data;
    src = buf;

    st = avctx->channels == 2;

    switch(1) {
	case 0://CODEC_ID_ADPCM_IMA_QT:
        n = (buf_size - 2);/* >> 2*avctx->channels;*/
        channel = c->channel;
        cs = &(c->status[channel]);
        /* (pppppp) (piiiiiii) */

        /* Bits 15-7 are the _top_ 9 bits of the 16-bit initial predictor value */
        cs->predictor = (*src++) << 8;
        cs->predictor |= (*src & 0x80);
        cs->predictor &= 0xFF80;

        /* sign extension */
        if(cs->predictor & 0x8000)
            cs->predictor -= 0x10000;

        CLAMP_TO_SHORT(cs->predictor);

        cs->step_index = (*src++) & 0x7F;

        //if (cs->step_index > 88) av_log(avctx, AV_LOG_ERROR, "ERROR: step_index = %i\n", cs->step_index);
        if (cs->step_index > 88) cs->step_index = 88;

        cs->step = step_table[cs->step_index];

        if (st && channel)
            samples++;

        for(m=32; n>0 && m>0; n--, m--) { /* in QuickTime, IMA is encoded by chuncks of 34 bytes (=64 samples) */
            *samples = adpcm_ima_expand_nibble(cs, src[0] & 0x0F, 3);
            samples += avctx->channels;
            *samples = adpcm_ima_expand_nibble(cs, (src[0] >> 4) & 0x0F, 3);
            samples += avctx->channels;
            src ++;
        }

        if(st) { /* handle stereo interlacing */
            c->channel = (channel + 1) % 2; /* we get one packet for left, then one for right data */
            if(channel == 1) { /* wait for the other packet before outputing anything */
                return src - buf;
            }
        }
        break;
	case 1://CODEC_ID_ADPCM_IMA_WAV:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;

        for(i=0; i<avctx->channels; i++){
            cs = &(c->status[i]);
            cs->predictor = *src++;
            cs->predictor |= (*src++) << 8;
            if(cs->predictor & 0x8000)
                cs->predictor -= 0x10000;
            CLAMP_TO_SHORT(cs->predictor);

	// XXX: is this correct ??: *samples++ = cs->predictor;

            cs->step_index = *src++;
            if (cs->step_index < 0) cs->step_index = 0;
            if (cs->step_index > 88) cs->step_index = 88;
            if (*src++) av_log(avctx, AV_LOG_ERROR, "unused byte should be null !!\n"); /* unused */
        }

        for(m=4; src < (buf + buf_size);) {
	    *samples++ = adpcm_ima_expand_nibble(&c->status[0], src[0] & 0x0F, 3);
            if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], src[4] & 0x0F, 3);
            *samples++ = adpcm_ima_expand_nibble(&c->status[0], (src[0] >> 4) & 0x0F, 3);
	    if (st) {
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], (src[4] >> 4) & 0x0F, 3);
		if (!--m) {
		    m=4;
		    src+=4;
		}
	    }
	    src++;
	}
        break;
	case 2://CODEC_ID_ADPCM_4XM:
        cs = &(c->status[0]);
        c->status[0].predictor= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        if(st){
            c->status[1].predictor= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        }
        c->status[0].step_index= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        if(st){
            c->status[1].step_index= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        }
        if (cs->step_index < 0) cs->step_index = 0;
        if (cs->step_index > 88) cs->step_index = 88;

        m= (buf_size - (src - buf))>>st;
        for(i=0; i<m; i++) {
	    *samples++ = adpcm_ima_expand_nibble(&c->status[0], src[i] & 0x0F, 4);
            if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], src[i+m] & 0x0F, 4);
            *samples++ = adpcm_ima_expand_nibble(&c->status[0], src[i] >> 4, 4);
	    if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], src[i+m] >> 4, 4);
	}

        src += m<<st;

        break;
	case 3://CODEC_ID_ADPCM_MS:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;
        n = buf_size - 7 * avctx->channels;
        if (n < 0)
            return -1;
        block_predictor[0] = clip(*src++, 0, 7);
        block_predictor[1] = 0;
        if (st)
            block_predictor[1] = clip(*src++, 0, 7);
        c->status[0].idelta = (int16_t)((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        src+=2;
        if (st){
            c->status[1].idelta = (int16_t)((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
            src+=2;
        }
        c->status[0].coeff1 = AdaptCoeff1[block_predictor[0]];
        c->status[0].coeff2 = AdaptCoeff2[block_predictor[0]];
        c->status[1].coeff1 = AdaptCoeff1[block_predictor[1]];
        c->status[1].coeff2 = AdaptCoeff2[block_predictor[1]];
        
        c->status[0].sample1 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        src+=2;
        if (st) c->status[1].sample1 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        if (st) src+=2;
        c->status[0].sample2 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        src+=2;
        if (st) c->status[1].sample2 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        if (st) src+=2;

        *samples++ = c->status[0].sample1;
        if (st) *samples++ = c->status[1].sample1;
        *samples++ = c->status[0].sample2;
        if (st) *samples++ = c->status[1].sample2;
        for(;n>0;n--) {
            *samples++ = adpcm_ms_expand_nibble(&c->status[0], (src[0] >> 4) & 0x0F);
            *samples++ = adpcm_ms_expand_nibble(&c->status[st], src[0] & 0x0F);
            src ++;
        }
        break;
	case 4://CODEC_ID_ADPCM_IMA_DK4:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;

        c->status[0].predictor = (int16_t)(src[0] | (src[1] << 8));
        c->status[0].step_index = src[2];
        src += 4;
        *samples++ = c->status[0].predictor;
        if (st) {
            c->status[1].predictor = (int16_t)(src[0] | (src[1] << 8));
            c->status[1].step_index = src[2];
            src += 4;
            *samples++ = c->status[1].predictor;
        }
        while (src < buf + buf_size) {

            /* take care of the top nibble (always left or mono channel) */
            *samples++ = adpcm_ima_expand_nibble(&c->status[0], 
                (src[0] >> 4) & 0x0F, 3);

            /* take care of the bottom nibble, which is right sample for
             * stereo, or another mono sample */
            if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], 
                    src[0] & 0x0F, 3);
            else
                *samples++ = adpcm_ima_expand_nibble(&c->status[0], 
                    src[0] & 0x0F, 3);

            src++;
        }
        break;
	case 5://CODEC_ID_ADPCM_IMA_DK3:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;

        c->status[0].predictor = (int16_t)(src[10] | (src[11] << 8));
        c->status[1].predictor = (int16_t)(src[12] | (src[13] << 8));
        c->status[0].step_index = src[14];
        c->status[1].step_index = src[15];
        /* sign extend the predictors */
        src += 16;
        diff_channel = c->status[1].predictor;

        /* the DK3_GET_NEXT_NIBBLE macro issues the break statement when
         * the buffer is consumed */
        while (1) {

            /* for this algorithm, c->status[0] is the sum channel and
             * c->status[1] is the diff channel */

            /* process the first predictor of the sum channel */
            DK3_GET_NEXT_NIBBLE();
            adpcm_ima_expand_nibble(&c->status[0], nibble, 3);

            /* process the diff channel predictor */
            DK3_GET_NEXT_NIBBLE();
            adpcm_ima_expand_nibble(&c->status[1], nibble, 3);

            /* process the first pair of stereo PCM samples */
            diff_channel = (diff_channel + c->status[1].predictor) / 2;
            *samples++ = c->status[0].predictor + c->status[1].predictor;
            *samples++ = c->status[0].predictor - c->status[1].predictor;

            /* process the second predictor of the sum channel */
            DK3_GET_NEXT_NIBBLE();
            adpcm_ima_expand_nibble(&c->status[0], nibble, 3);

            /* process the second pair of stereo PCM samples */
            diff_channel = (diff_channel + c->status[1].predictor) / 2;
            *samples++ = c->status[0].predictor + c->status[1].predictor;
            *samples++ = c->status[0].predictor - c->status[1].predictor;
        }
        break;
	case 6://CODEC_ID_ADPCM_IMA_WS:
        /* no per-block initialization; just start decoding the data */
        while (src < buf + buf_size) {

            if (st) {
                *samples++ = adpcm_ima_expand_nibble(&c->status[0], 
                    (src[0] >> 4) & 0x0F, 3);
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], 
                    src[0] & 0x0F, 3);
            } else {
                *samples++ = adpcm_ima_expand_nibble(&c->status[0], 
                    (src[0] >> 4) & 0x0F, 3);
                *samples++ = adpcm_ima_expand_nibble(&c->status[0], 
                    src[0] & 0x0F, 3);
            }

            src++;
        }
        break;
	case 7://CODEC_ID_ADPCM_XA:
        c->status[0].sample1 = c->status[0].sample2 = 
        c->status[1].sample1 = c->status[1].sample2 = 0;
        while (buf_size >= 128) {
            xa_decode(samples, src, &c->status[0], &c->status[1], 
                avctx->channels);
            src += 128;
            samples += 28 * 8;
            buf_size -= 128;
        }
        break;
		/*
	case 8://CODEC_ID_ADPCM_EA:
        samples_in_chunk = LE_32(src);
        if (samples_in_chunk >= ((buf_size - 12) * 2)) {
            src += buf_size;
            break;
        }
        src += 4;
        current_left_sample = (int16_t)LE_16(src);
        src += 2;
        previous_left_sample = (int16_t)LE_16(src);
        src += 2;
        current_right_sample = (int16_t)LE_16(src);
        src += 2;
        previous_right_sample = (int16_t)LE_16(src);
        src += 2;

        for (count1 = 0; count1 < samples_in_chunk/28;count1++) {
            coeff1l = ea_adpcm_table[(*src >> 4) & 0x0F];
            coeff2l = ea_adpcm_table[((*src >> 4) & 0x0F) + 4];
            coeff1r = ea_adpcm_table[*src & 0x0F];
            coeff2r = ea_adpcm_table[(*src & 0x0F) + 4];
            src++;

            shift_left = ((*src >> 4) & 0x0F) + 8;
            shift_right = (*src & 0x0F) + 8;
            src++;

            for (count2 = 0; count2 < 28; count2++) {
                next_left_sample = (((*src & 0xF0) << 24) >> shift_left);
                next_right_sample = (((*src & 0x0F) << 28) >> shift_right);
                src++;

                next_left_sample = (next_left_sample + 
                    (current_left_sample * coeff1l) + 
                    (previous_left_sample * coeff2l) + 0x80) >> 8;
                next_right_sample = (next_right_sample + 
                    (current_right_sample * coeff1r) + 
                    (previous_right_sample * coeff2r) + 0x80) >> 8;
                CLAMP_TO_SHORT(next_left_sample);
                CLAMP_TO_SHORT(next_right_sample);

                previous_left_sample = current_left_sample;
                current_left_sample = next_left_sample;
                previous_right_sample = current_right_sample;
                current_right_sample = next_right_sample;
                *samples++ = (unsigned short)current_left_sample;
                *samples++ = (unsigned short)current_right_sample;
            }
        }
        break;*/
	case 9://CODEC_ID_ADPCM_IMA_SMJPEG:
        c->status[0].predictor = *src;
        src += 2;
        c->status[0].step_index = *src++;
        src++;  /* skip another byte before getting to the meat */
        while (src < buf + buf_size) {
            *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                *src & 0x0F, 3);
            *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                (*src >> 4) & 0x0F, 3);
            src++;
        }
        break;
	case 10://CODEC_ID_ADPCM_CT:
	while (src < buf + buf_size) {
            if (st) {
                *samples++ = adpcm_ct_expand_nibble(&c->status[0], 
                    (src[0] >> 4) & 0x0F);
                *samples++ = adpcm_ct_expand_nibble(&c->status[1], 
                    src[0] & 0x0F);
            } else {
                *samples++ = adpcm_ct_expand_nibble(&c->status[0], 
                    (src[0] >> 4) & 0x0F);
                *samples++ = adpcm_ct_expand_nibble(&c->status[0], 
                    src[0] & 0x0F);
            }
	    src++;
        }
        break;
    default:
        return -1;
    }
    *data_size = (uint8_t *)samples - (uint8_t *)data;
    return src - buf;
}



#ifdef CONFIG_ENCODERS
#define ADPCM_ENCODER(id,name)                  \
AVCodec name ## _encoder = {                    \
    #name,                                      \
    CODEC_TYPE_AUDIO,                           \
    id,                                         \
    sizeof(ADPCMContext),                       \
    adpcm_encode_init,                          \
    adpcm_encode_frame,                         \
    adpcm_encode_close,                         \
    NULL,                                       \
};
#else
#define ADPCM_ENCODER(id,name)
#endif

#ifdef CONFIG_DECODERS
#define ADPCM_DECODER(id,name)                  \
AVCodec name ## _decoder = {                    \
    #name,                                      \
    CODEC_TYPE_AUDIO,                           \
    id,                                         \
    sizeof(ADPCMContext),                       \
    adpcm_decode_init,                          \
    NULL,                                       \
    NULL,                                       \
    adpcm_decode_frame,                         \
};
#else
#define ADPCM_DECODER(id,name)
#endif

#define ADPCM_CODEC(id, name)                   \
ADPCM_ENCODER(id,name) ADPCM_DECODER(id,name)

ADPCM_CODEC(CODEC_ID_ADPCM_IMA_QT, adpcm_ima_qt);
ADPCM_CODEC(CODEC_ID_ADPCM_IMA_WAV, adpcm_ima_wav);
ADPCM_CODEC(CODEC_ID_ADPCM_IMA_DK3, adpcm_ima_dk3);
ADPCM_CODEC(CODEC_ID_ADPCM_IMA_DK4, adpcm_ima_dk4);
ADPCM_CODEC(CODEC_ID_ADPCM_IMA_WS, adpcm_ima_ws);
ADPCM_CODEC(CODEC_ID_ADPCM_IMA_SMJPEG, adpcm_ima_smjpeg);
ADPCM_CODEC(CODEC_ID_ADPCM_MS, adpcm_ms);
ADPCM_CODEC(CODEC_ID_ADPCM_4XM, adpcm_4xm);
ADPCM_CODEC(CODEC_ID_ADPCM_XA, adpcm_xa);
ADPCM_CODEC(CODEC_ID_ADPCM_ADX, adpcm_adx);
ADPCM_CODEC(CODEC_ID_ADPCM_EA, adpcm_ea);
ADPCM_CODEC(CODEC_ID_ADPCM_CT, adpcm_ct);

#undef ADPCM_CODEC