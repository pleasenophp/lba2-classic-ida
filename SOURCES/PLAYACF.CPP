#include "c_extern.h"
#include <smacker/smacker.h>

#pragma pack(8)
#include	"soloud.h"
#pragma pack(1)

#include "SmackerStream.h"

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
//			if( PlayAcf( getname ) == K_ESC )	break ;
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

//extern	void	CopyFrameDoubleXY( U8 *ptrsrc ) ;

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

S32	PlayAcf( char *name )
{
	// xesf
	U32	i, j, c, h, w, f;
	S32	k, n;
	S32	ret;
	U32 fps, timer, size;
	smk smkObject;
	double usf;
	U8	a_t, a_c[SMACKER_TRACK_SIZE], a_d[SMACKER_TRACK_SIZE];
	U32	a_r[SMACKER_TRACK_SIZE];
	U8* decompbuf = nullptr;
	U8* frame = nullptr;
	U8* pal = nullptr;
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
	for (c = 0; c < f; c++)
	{
		ReadNextAudioChunk(smkObject, a_t, smackflags);

		U32 offset = 0;
		frame = (U8 *)smk_get_video(smkObject);
		pal = (U8*)smk_get_palette(smkObject);

		// very hacky smacker full screen
		// TODO - improve it with memcpy
		memset(dest, 0, 640 * 480);

		offset = 640 * 40;

		for (j = 0; j < h; j++) {
			U32 innerOffset = 0;
			for (k = 0; k < w; k++) {
				*(dest + offset) = *(frame);
				*(dest + offset + 1) = *(frame++);
				offset += 2;
			}

			innerOffset = offset - 640;
			for (k = 0; k < w; k++) {
				*(dest + offset) = *(dest + innerOffset);
				*(dest + offset + 1) = *(dest + innerOffset + 1);
				offset += 2;
				innerOffset += 2;
			}
		}

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

#if 0
	Smack		*smk	      	;
	U32		frame	      	;
	S32		n 		;
	S32		ret 		;
	U8		*decompbuf	;
	U32		smackflags	;
#ifdef	LBA_EDITOR
	char		name_acf[256] 	;

	ShowMouse( 0 ) ;

	smackflags = SMACKTRACK1|SMACKNEEDVOLUME ;
#else
	smackflags = SMACKFILEHANDLE|SMACKTRACK1|SMACKNEEDVOLUME ;
#endif

	// si c'est l'intro, on joue la track de la langue choisie
	if( !strnicmp(name,"INTRO",5) )
	{
#ifdef	CDROM
		if( FlagSpeak )
		{
			switch( LanguageCD )
			{
				case 0:  // English
					smackflags |= SMACKTRACK4 ;
					break ;

				case 1:	// Français
					smackflags |= SMACKTRACK2 ;
					break ;

				case 2:	// Deutsch
					smackflags |= SMACKTRACK3 ;
					break ;

//				default:// other ...
					// no spoken track
			}
		}
#else
		smackflags |= SMACKTRACK4 ;// English and other ...
#endif
	}

	// Ah ? pas de son ? tiens donc ... gicle-moi les tracks !
/*	if( !Sample_Driver_Enabled )
	{
		smackflags &= ~(SMACKTRACKS|SMACKNEEDVOLUME) ;
	}
*/
	n = GetNumAcf( name ) ;

	if( n==-1 )	return 0 ;

	{
		S32	var	;
		S32	bit	;

		var = n/16	;
		if( n>=32 )
		{
			var = FLAG_ACF2 ;
			bit = 1<<(n-32) ;
		}
		else
		{
			var = FLAG_ACF 	;
			bit = 1<<n 	;
		}

#ifdef	LBA_EDITOR
		ListVarGame[FLAG_ACF+(n/16)].Value |= (S16)(1<<(n&15)) ;
#else
		ListVarGame[FLAG_ACF+(n/16)] |= (S16)(1<<(n&15)) ;
#endif
	}

	StopMusic() ;

	SmackResetMemory() ;

#ifdef	LBA_EDITOR
	strcpy( name_acf, PathAcf ) ;
	strcat( name_acf, name ) ;
	AddExt( name_acf, ".SMK" ) ;

	smk = SmackOpen( name_acf, smackflags, SMACKAUTOEXTRA ) ;
#else
//	Fd = OpenRead(name_acf)	;// Ouvrir ressource + Seek

	// ouvre la ressource et positionne le handle à l'offet ou se trouve
	// la sequence
	if( !HQF_Init( PathAcf, n ) )
#if defined(DEBUG_TOOLS)||defined(TEST_TOOLS)||!defined(CDROM)
		return 0 ;
#else
		TheEnd( PROGRAM_OK, MessageNoCD ) ;
#endif

	smk = SmackOpen( (char*)HQF_File, smackflags, SMACKAUTOEXTRA ) ;
#endif

	if( !smk )
#if defined(DEBUG_TOOLS)||defined(TEST_TOOLS)||!defined(CDROM)
		return 0 ;
#else
		TheEnd( PROGRAM_OK, MessageNoCD ) ;
#endif

	decompbuf = (U8*)radmalloc( 320*200 )	;

	if( !decompbuf )
	{
		SmackClose( smk ) ;
#ifndef	LBA_EDITOR
		HQF_Close() ;
#endif
		return 0 ;
	}

	memset( decompbuf, 0, 320*200 ) ;

	SmackToBuffer( smk, 0, 0, 320, 200, decompbuf, FALSE ) ;

//	if( Sample_Driver_Enabled )
	{
		S32	vol ;

#ifdef	YAZ_WIN32
		if( UseWaveMixer )
		{
			vol = SampleVolume ;
		}
		else
#endif
		{
#ifdef	LBA_EDITOR
			vol = SampleVolume ;
#else
			vol = BoundRegleTrois( 0, MasterVolume, 127, SampleVolume ) ;
#endif
		}

		vol = (vol==0?vol:(vol+1))*512 ;// 0..65536

		SmackVolumePan( smk, SMACKTRACKS, vol, 32768 ) ;
	}

	SetBlackPal();

	BoxReset()  ;// pour etre sûr de ne pas utiliser Screen !
	Cls() ;
	BoxUpdate() ;

	InitWaitNoKey()			;
	InitWaitNoInput( I_JOY|I_FIRE|I_MENUS ) ;

	for( frame=0; frame<smk->Frames; frame++ )
	{
		SmackDoFrame( smk ) ;

#ifndef	LBA_EDITOR
		if( !VideoFullScreen )
		{
			if( smk->NewPalette )
			{
#ifndef	LBA_EDITOR
				FlagBlackPal = FALSE ;
#endif
				Palette(smk->Palette) ;
			}
#ifdef	YAZ_WIN32
			BlitBox( (void*)Phys, (void*)decompbuf ) ;
#else
			BlitBoxVESA( (void*)Phys, (void*)decompbuf ) ;
#endif

		}
		else
#endif
		{
			CopyFrameDoubleXY( (void*)decompbuf ) ;
			BoxStaticAdd( 0, 40, 639, 439 ) ;
		}

#ifdef	LBA_EDITOR
		if( FlagInfos & INFO_FRAME_SPEED )
		{
			GraphPrintf(FALSE, 0,0, "Frame: %d", frame++);
		}
#endif

#ifndef	LBA_EDITOR
		if( VideoFullScreen )
#endif
		{
			if( smk->NewPalette )
			{
#ifndef	LBA_EDITOR
				FlagBlackPal = FALSE ;
#endif
				// Pour eviter flash sur frame precedente
//				Cls()  ;
//				BoxUpdate() ;

				Palette(smk->Palette) ;
			}

			BoxUpdate() ;
		}

		if( frame!=smk->Frames )
		{
			SmackNextFrame( smk ) ;
		}

		do
		{
			MyGetInput() ;
			ManageTime() ;

#ifndef	LBA_EDITOR
			CheckSaveLogPcx( smk->Palette ) ;// F9: Sauve Screen dans PATH_PCX_SAVE
#endif

			if( (MyKey==K_ESC OR (Input&I_MENUS))
#ifndef	LBA_EDITOR
			AND (!FlagPlayAcf OR frame>290)
#endif
			)
			{
				goto fin_play ;
			}
		}
		while( SmackWait( smk ) ) ;
	}

fin_play:

	SmackClose( smk ) ;

#ifndef	LBA_EDITOR
	HQF_Close() ;
	LastIdxPalette = 0 ;// pour reforcer le chargement du .XPL (cf ChoicePalette())
#endif

	radfree( decompbuf ) ;

	FadeToBlack( smk->Palette ) ;
	Cls() ;
	BoxStaticFullflip() ;
	SetBlackPal() ;

	ret = MyKey ;

#ifdef	LBA_EDITOR
	SetMouseBox( 0,0, 639,479 ) ;
#endif

	InitWaitNoKey()			;
	InitWaitNoInput( I_MENUS ) ;

#ifdef	LBA_EDITOR
	ShowMouse( 1 ) ;
#endif

	return ret ;
#endif
}

//---------------------------------------------------------------------------
