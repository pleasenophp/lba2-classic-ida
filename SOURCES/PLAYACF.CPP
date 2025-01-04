#include "c_extern.h"
#include <string>
#include <smacker/smacker.h>

#pragma pack(8)
#include "soloud.h"
#include "SmackerStream.h"
#pragma pack(1)


using namespace Ida;

char		PathAcf[ADELINE_MAX_PATH] ;

char		*ListAcf ;
char		*EndListAcf ;

#ifdef	LBA_EDITOR
#define	SIZE_BUFFER_SMACK	1000000L
U8  	*BufferSmack ;
#else
U8  	*BufferSmack ;// initialisé dans MEM.CPP
#endif

#define SMACKER_TRACK_SIZE 7

struct soundHandle2
{
	U32 soloudHandle = 0;
	SmackerStream *soloudStream = nullptr;
};

soundHandle2 soundHandleMapping[SMACKER_TRACK_SIZE]; // smacker tracks

//---------------------------------------------------------------------------

/***************************************************************************\
|*									   *|
|*				INITS					   *|
|*									   *|
\***************************************************************************/

//---------------------------------------------------------------------------
void	InitAcf( void )
{
#if defined(DEBUG_TOOLS)||defined(TEST_TOOLS)
	char    *path ;

	path = DefFileBufferReadString( "PathAcf" ) ;

	if( path AND strcmp(path,"") )
	{
		strcpy( PathAcf, path ) ;
		if( PathAcf[strlen(PathAcf)-1]!='\\'
		AND PathAcf[strlen(PathAcf)-1]!=':' )
			strcat( PathAcf, "\\" ) ;
	}
	else
#endif
	{
#if defined(ONE_GAME_DIRECTORY)&&defined(CDROM)

#if defined(DEBUG_TOOLS)||defined(TEST_TOOLS)
		if( !CDDrive )
		{
			strcpy( PathAcf, PATH_ACF ) ;
		}
		else	sprintf( PathAcf, "%c:\\LBA2\\VIDEO\\VIDEO.HQR", CDDrive ) ;
#else
		switch( DistribVersion )
		{
			case UNKNOWN_VERSION:
			case EA_VERSION:
				//xesf sprintf( PathAcf, "%c:\\LBA2\\VIDEO\\VIDEO.HQR", CDDrive ) ;
				sprintf(PathAcf, "%sVIDEO\\VIDEO.HQR", PATH_RESSOURCE);
				break ;

			default:
				//xesf sprintf( PathAcf, "%c:\\TWINSEN\\VIDEO\\VIDEO.HQR", CDDrive ) ;
				sprintf(PathAcf, "%sVIDEO\\VIDEO.HQR", PATH_RESSOURCE);
		}

		//yaz if( !FileSize( PathAcf ) )	TheEnd( PROGRAM_OK, MessageNoCD ) ;
#endif
#else
		// default path Acf
		strcpy( PathAcf, PATH_ACF ) ;
#endif
	}

	ListAcf = (char *)LoadMalloc_HQR( RESS_HQR_NAME, RESS_ACFLIST ) ;
	if( !ListAcf )	TheEndCheckFile( RESS_HQR_NAME ) ;

	EndListAcf = ListAcf + LoadMallocFileSize ;

#ifdef	LBA_EDITOR
	// dispo Screen,ScreenAux,BufSpeak
	SmackRegisterMemory( Screen, (ModeDesiredX*ModeDesiredY+RECOVER_AREA)*3 ) ;

	BufferSmack = (U8*)Malloc( SIZE_BUFFER_SMACK ) ;
	if( !BufferSmack )	TheEnd( NOT_ENOUGH_MEM, "BufferSmack (InitAcf())" ) ;

	SmackRegisterMemory( (void*)BufferSmack, SIZE_BUFFER_SMACK ) ;
#else
#if 0 // yaz
	SmackRegisterMemory( MainBuffer, SizeOfMemAllocated ) ;
#endif
#endif
}

//---------------------------------------------------------------------------

char	*PtrAcf ;

char	*GetAcfName( char *name )
{
	char	*ptr = name ;
	char	c ;

	do	/* saute cr/lf espace */
	{
		if( PtrAcf>EndListAcf )
		{
			PtrAcf--  ;
			return 0L ;
		}

		c = *PtrAcf++ ;
	}
	while( c <= 32 ) ;

	do
	{
		*ptr++ = c ;
		c = *PtrAcf++ ;
	}
	while( c > 32 ) ;

	if( PtrAcf>EndListAcf )
	{
		PtrAcf--  ;
		return 0L ;
	}

	if( c == 26 )	PtrAcf-- ;

	*ptr++ = 0 ;
	return name ;
}

//---------------------------------------------------------------------------

S32	GetNumAcf( char *name )
{
	S32	n = 0 ;
	char	getname[15] ;
	char	*ptr ;

	PtrAcf = ListAcf ;

	ptr = strchr(name,'.') ;
	if( ptr )	*ptr = 0 ;

	while( GetAcfName( getname ) )
	{
		ptr = strchr(getname,'.') ;
		if( ptr )	*ptr = 0 ;

		if( !stricmp(name,getname) )	return n ;
		n++ ;
	}

	return -1 ;
}

//---------------------------------------------------------------------------

void	PlayAllAcf( void )
{
	S32	firsttime = TRUE ;
	S32	n = 0 ;
	S32	var = FLAG_ACF ;
	char	getname[13] ;

	PtrAcf = ListAcf ;

#if defined(LBA_EDITOR)||defined(CDROM)
	FadeOutVolumeMusic() ;
#endif

	while( GetAcfName( getname ) )
	{
#ifdef	LBA_EDITOR
		if( (ListVarGame[var].Value & (1<<n))
#else
		if( (ListVarGame[var] & (1<<n))
#endif
		AND (strnicmp(getname,"INTRO",5)) )// on ne rejoue pas l'intro
		{
			if( !FlagFade )
			{
				if( firsttime )
				{
					FadeToBlackAndSamples( PtrPal ) ;
					firsttime = FALSE ;
				}
				else	FadeToBlack( PtrPal ) ;
			}

			FlagFade = TRUE ;

			PlayAcf( getname ) ;
		}

		n++ ;

		if( n>=16 )
		{
			var++ ;
			n = 0 ;
		}
	}

#if defined(LBA_EDITOR)||defined(CDROM)
	FadeInVolumeMusic() ;
#endif

	FirstTime = AFF_ALL_FLIP ;
}

//---------------------------------------------------------------------------

/***************************************************************************\
|*									   *|
|*				MAIN					   *|
|*									   *|
\***************************************************************************/


static void StartPlayAudio(SoLoud::Soloud *soloud, U8 availableTracks, U8 *channelsPerTrack, U8 *bitDepthsPerTrack, U32 *sampleRatePerTrack, S32 smackFlags) 
{
	for (int i = 0; i < SMACKER_TRACK_SIZE; i++)
	{
		if (availableTracks & (1 << i) && (1 << i) & smackFlags)
		{
			SmackerStream *smackerStream = new SmackerStream(bitDepthsPerTrack[i], sampleRatePerTrack[i], channelsPerTrack[i]);
			soundHandleMapping[i].soloudStream = smackerStream;
			soundHandleMapping[i].soloudHandle = soloud->play(*smackerStream, -1, 0, false, 0);
		}
	}
}

static void ReadNextAudioChunk(const smk smkObject, U8 availableTracks, S32 smackFlags) {
	for (int i = 0; i < SMACKER_TRACK_SIZE; i++)
	{
		if (!soundHandleMapping[i].soloudStream) {
			continue;
		}

		long sampleCount = smk_get_audio_size(smkObject, i);
		const unsigned char *chunk = smk_get_audio(smkObject, i);
		soundHandleMapping[i].soloudStream->addNextChunk(chunk, sampleCount);
	}
}

static void StopAudio(SoLoud::Soloud *soloud) 
{
	for (int i = 0; i < SMACKER_TRACK_SIZE; i++)
	{
		soloud->stop(soundHandleMapping[i].soloudHandle);
		if (soundHandleMapping[i].soloudStream) {
			delete soundHandleMapping[i].soloudStream;
			soundHandleMapping[i].soloudStream = nullptr;
		}
		soundHandleMapping[i].soloudHandle = 0;
	}
}

static void ReadNextVideoFrame(const smk smkObject, U8 *buffer, U8 **pal)
{
	constexpr U32 width = 320;
	constexpr U32 height = 200;

	constexpr U32 screenWidth = 640;
	constexpr U32 screenHeight = 480;

	constexpr U32 videoStartY = 40;

	U8* frame = (U8*)smk_get_video(smkObject);
	*pal = (U8*)smk_get_palette(smkObject);

	memset(buffer, 0, screenWidth * screenHeight);

	auto targetRow = buffer + videoStartY * screenWidth;
	for (U32 y = 0; y < height; y++) {
		auto sourceRow = frame + y * width;

		for (U32 x = 0; x < width; x++) {
			targetRow[2 * x] = sourceRow[x];
			targetRow[2 * x + 1] = sourceRow[x];
		}

		memcpy(targetRow + screenWidth, targetRow, screenWidth);

		targetRow += screenWidth * 2;
	}
}

S32	PlayAcf( char *name )
{
	// xesf
	U32	c, h, w, f;
	S32	n;
	S32	ret;
	U32 fps, timer, size;
	smk smkObject;
	double usf;
	U8	a_t, a_c[SMACKER_TRACK_SIZE], a_d[SMACKER_TRACK_SIZE];
	U32	a_r[SMACKER_TRACK_SIZE];
	U8* decompbuf = nullptr;
	S32 smackflags = SMK_AUDIO_TRACK_0;
	U8* dest = nullptr;
	SoLoud::Soloud* soloud = nullptr;
	S32 rate = 22050;
	S32 chans = 2;

	if (!strnicmp(name, "INTRO", 5))
	{
		if (FlagSpeak)
		{
			switch (LanguageCD)
			{
			case 0:  // English
				smackflags |= SMK_AUDIO_TRACK_3;
				break;

			case 1:	// Français
				smackflags |= SMK_AUDIO_TRACK_1;
				break;

			case 2:	// Deutsch
				smackflags |= SMK_AUDIO_TRACK_2;
				break;
			}
		}
	}

	n = GetNumAcf(name);

	if (n == -1)	return 0;

	{
		S32	var;
		S32	bit;

		var = n / 16;
		if (n >= 32)
		{
			var = FLAG_ACF2;
			bit = 1 << (n - 32);
		}
		else
		{
			var = FLAG_ACF;
			bit = 1 << n;
		}

	}

	StopMusic();

	decompbuf = (U8 *)LoadMalloc_HQR(PathAcf, n);
	size = LoadMallocFileSize;

	if (!size)
		TheEnd(PROGRAM_OK, MessageNoCD);

	smkObject = smk_open_memory(decompbuf, size);

	if (!smkObject)
		TheEnd(PROGRAM_OK, MessageNoCD);

	if (!decompbuf)
	{
		smk_close(smkObject);
		return 0;
	}

	SetBlackPal();

	BoxReset();// pour etre sûr de ne pas utiliser Screen !
	Cls();
	BoxUpdate();

	InitWaitNoKey();
	InitWaitNoInput(I_JOY | I_FIRE | I_MENUS);

	smk_info_all(smkObject, NULL, &f, &usf);
	smk_info_video(smkObject, &w, &h, NULL);
	if (w != 320 || h != 200) 
	{
		LogPrintf("ERROR: only videos of format 320x200 are supported at the moment; Tried to play %s (%d) with resolution %dx%d", name, n, w, h);
		TheEnd(PROGRAM_FAIL, "Unsupported video format");
	}

	smk_info_audio(smkObject, &a_t, a_c, a_d, a_r);
	fps = (U32)(1000000.0 / usf);

	soloud = new SoLoud::Soloud();
	soloud->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::BACKENDS::AUTO, rate, 0, chans);

	// video & audio
	smk_enable_all(smkObject, SMK_VIDEO_TRACK | a_t);
	smk_first(smkObject);
	
	dest = (U8*)malloc(640 * 480);

	StartPlayAudio(soloud, a_t, a_c, a_d, a_r, smackflags);

	timer = TimerRefHR + (1000/fps);

	U8* pal;
	for (c = 0; c < f; c++)
	{
		ReadNextAudioChunk(smkObject, a_t, smackflags);

		ReadNextVideoFrame(smkObject, dest, &pal);
		CopyBlock(0, 0, 640, 480, dest, 0, 0, Log);
		PaletteSync(pal, true);

		do
		{
			MyGetInput();
			ManageTime();

			if ((MyKey == K_ESC OR(Input & I_MENUS)))
			{
				goto fin_play;
			}
		} while (timer > TimerRefHR);

		timer = TimerRefHR + (1000 / fps);

		smk_next(smkObject);
	}

fin_play:
	//FadeToBlack(smk->Palette);
	Cls();
	BoxStaticFullflip();
	SetBlackPal();

	StopAudio(soloud);
	soloud->deinit();
	delete soloud;

	smk_close(smkObject);
	free(dest);
	Free(decompbuf);

	ret = MyKey;

	InitWaitNoKey();
	InitWaitNoInput(I_MENUS);

	return ret;
}

