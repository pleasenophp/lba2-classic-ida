#include	"c_extern.h"


#ifndef	LBA_EDITOR
void	InitTabTracks( void ) ;// voir plus bas...
#endif

/*──────────────────────────────────────────────────────────────────────────*/

S32	InitCD(char *name)
{
#ifndef	LBA_EDITOR
	InitTabTracks();
#endif

	return TRUE;
}

 /*══════════════════════════════════════════════════════════════════════════*
			  █  █    ██▄ █ █▀▀▀▀ █     █▀▀▀▀
		      ▄▄  █  ██   ██▀██ ██ ▀█ ██    ██▀▀
		      ▀▀▀▀▀  ▀▀   ▀▀  ▀ ▀▀▀▀▀ ▀▀▀▀▀ ▀▀▀▀▀
 *══════════════════════════════════════════════════════════════════════════*/
/*──────────────────────────────────────────────────────────────────────────*/

char	PathJingle[ADELINE_MAX_PATH] ;

U32	JingleVolume = DEF_JINGLE_VOLUME ;

#define	FIRST_JINGLE 2
#define	LAST_JINGLE	 26

#define	NB_JINGLE	 26

/* This can be useful for non-GOG/Steam data version
char	ListJingle[NB_JINGLE][9] = {
	"",		// pour interfacer avec la music CD (car Track 0 == NO_TRACK)
	"TADPCM1",
	"TADPCM2",
	"TADPCM3",
	"TADPCM4",
	"TADPCM5",
	"JADPCM01",
	"TADPCM6",	//xesf Track6.wav
	"JADPCM02",
	"JADPCM03",
	"JADPCM04",
	"JADPCM05",
	"JADPCM06",
	"JADPCM07",
	"JADPCM08",
	"JADPCM09",
	"JADPCM10",
	"JADPCM11",
	"JADPCM12",
	"JADPCM13",
	"JADPCM14",
	"JADPCM15",
	"JADPCM16",
	"JADPCM17",
	"JADPCM18",
	"LOGADPCM",
	} ;
*/

char	ListJingle[NB_JINGLE][9] = {
	"",		// pour interfacer avec la music CD (car Track 0 == NO_TRACK)
	"track1",
	"track2",
	"track3",
	"track4",
	"track5",
	"JADPCM01",
	"track6",	//xesf Track6.wav
	"JADPCM02",
	"JADPCM03",
	"JADPCM04",
	"JADPCM05",
	"JADPCM06",
	"JADPCM07",
	"JADPCM08",
	"JADPCM09",
	"JADPCM10",
	"JADPCM11",
	"JADPCM12",
	"JADPCM13",
	"JADPCM14",
	"JADPCM15",
	"JADPCM16",
	"JADPCM17",
	"JADPCM18",
	"LOGADPCM",
	} ;

/*──────────────────────────────────────────────────────────────────────────*/

void InitJingle(void)
{
	char *path;

	OpenStream(); // init Stream System

	// fix path jingles
	path = DefFileBufferReadString("PathJingle");

	if (path AND strcmp(path, ""))
	{
		strcpy(PathJingle, path);
		if (PathJingle[strlen(PathJingle)-1] != '\\' AND PathJingle[strlen(PathJingle)-1] != ':')
			strcat(PathJingle, "\\");
	}
	else
	{
		strcpy(PathJingle, PATH_JINGLE);
	}

	SetVolumeJingle(JingleVolume);
}

/*──────────────────────────────────────────────────────────────────────────*/

S32	GetNumJingle( char *name )
{
	char	filename[9] ;
	S32	n ;

	if( name AND strcmp(name,"") )
	{
		strcpy( filename, GetFileName(name) ) ;

		for( n=1; n<NB_JINGLE; n++ )
		{
			if( !stricmp(ListJingle[n],filename) )
			{
				return (n+FIRST_JINGLE-1) ;// -1 car 0==NO_TRACK
			}
		}
	}

	return 0 ;
}

/*──────────────────────────────────────────────────────────────────────────*/

void	PlayJingle( S32 num )
{
	char	name_jingle[ADELINE_MAX_PATH] ;

	if( num>=FIRST_JINGLE
	AND num<=LAST_JINGLE
	AND JingleVolume>0 )
	{
		LockTimer() ;

		num -= FIRST_JINGLE ;
		num++ ;// car track 0 == NO_TRACK

#ifdef ONE_GAME_DIRECTORY
		// tente d'abord de jouer le jingle dans le sous-répertoire
		// MUSIC
		strcpy( name_jingle, "MUSIC\\" ) ;
		strcat( name_jingle, ListJingle[num] ) ;
		AddExt( name_jingle, ".WAV" ) ;

		if( !FileSize( name_jingle ) )
#endif
		{
			strcpy( name_jingle, PathJingle ) ;
			strcat( name_jingle, ListJingle[num] ) ;
			AddExt( name_jingle, ".OGG" ) ;
		}

		// a remplacer par GetNumJingle() ?
		if( stricmp(name_jingle,StreamName()) )
		{
			PlayStream( name_jingle ) ;
		}

		UnlockTimer() ;
	}
}

/*══════════════════════════════════════════════════════════════════════════*
			 █▄ ▄█ █   █ ██▀▀▀  █    █▀▀▀▀
			 ██▀ █ ██  █ ▀▀▀▀█  ██   ██
			 ▀▀  ▀ ▀▀▀▀▀ ▀▀▀▀▀  ▀▀   ▀▀▀▀▀
 *══════════════════════════════════════════════════════════════════════════*/
/*──────────────────────────────────────────────────────────────────────────*/


// Tableau d'indirection au cas où l'ordre change sur le CD après la saisie
// dans l'outil

#define	JINGLE	0x80	// flag indiquant une track streamée
#define	MUSIC	0x7F	// pour avoir le numero de musique

#define	FIRST_CD_TRACK		6
#define	FIRST_CD_TRACK_US	0

U8	FirstCDTrack = FIRST_CD_TRACK ;

U8	TrackCD[] = {
JINGLE| 2,	// Track01.wav
JINGLE| 3,	// Track02.wav
JINGLE| 4,	// Track03.wav
JINGLE| 5,	// Track04.wav
JINGLE| 6,	// Track05.wav
JINGLE| 7,	// Jingle01.wav
JINGLE| 8,	// Track06.wav // xesf
JINGLE| 9,	// Jingle02.wav
JINGLE|10,	// Jingle03.wav
JINGLE|11,	// Jingle04.wav
JINGLE|12,	// Jingle05.wav
JINGLE|13,	// Jingle06.wav
JINGLE|14,	// Jingle07.wav
JINGLE|15,	// Jingle08.wav
JINGLE|16,	// Jingle09.wav
JINGLE|17,	// Jingle10.wav
JINGLE|18,	// Jingle11.wav
JINGLE|19,	// Jingle12.wav
JINGLE|20,	// Jingle13.wav
JINGLE|21,	// Jingle14.wav
JINGLE|22,	// Jingle15.wav
JINGLE|23,	// Jingle16.wav
JINGLE|24,	// Jingle17.wav
JINGLE|25,	// Jingle18.wav
JINGLE|26,	// LogAdpcm.wav
	} ;

U8	TrackCDUS[] = {
	2,	// Track01.wav
	3,	// Track02.wav
	4,	// Track03.wav
	5,	// Track04.wav
	6,	// Track05.wav
	7,	// Jingle01.wav
	8,	// Track06.wav
JINGLE| 9,	// Jingle02.wav
JINGLE|10,	// Jingle03.wav
JINGLE|11,	// Jingle04.wav
JINGLE|12,	// Jingle05.wav
JINGLE|13,	// Jingle06.wav
JINGLE|14,	// Jingle07.wav
JINGLE|15,	// Jingle08.wav
JINGLE|16,	// Jingle09.wav
JINGLE|17,	// Jingle10.wav
JINGLE|18,	// Jingle11.wav
JINGLE|19,	// Jingle12.wav
JINGLE|20,	// Jingle13.wav
JINGLE|21,	// Jingle14.wav
JINGLE|22,	// Jingle15.wav
JINGLE|23,	// Jingle16.wav
JINGLE|24,	// Jingle17.wav
JINGLE|25,	// Jingle18.wav
JINGLE|26,	// LogAdpcm.wav
	} ;

U8	*PtrTrackCD ;

#define	TEST_MUSIC_TEMPO	(2*1000)	// 2s

S32	NextMusic = -1 ;
S32	StopLastMusic = FALSE ;
S32	NextMusicTimer ;

/*──────────────────────────────────────────────────────────────────────────*/
#if !defined(LBA_EDITOR)
void	InitTabTracks( void )
{
	switch( DistribVersion )
	{
		case UNKNOWN_VERSION:
		case EA_VERSION:
			PtrTrackCD = TrackCD;
			FirstCDTrack = FIRST_CD_TRACK;
			break ;

		default:
			PtrTrackCD = TrackCDUS;
			FirstCDTrack = FIRST_CD_TRACK_US;
	}
}
#endif

/*──────────────────────────────────────────────────────────────────────────*/
// Cette fonction doit orienter la musique soit sur une Track Audio, soit
// sur du Streaming ADPCM

void	PlayMusic( S32 num, S32 playit )
{
	NextMusic = -1 ;

	PIRACY_PROTECT() ;

	if( num!=-1 )
	{
		S32 cur ;

		LockTimer() ;

		if (playit OR StopLastMusic) cur = 0;
		else cur = GetMusic();

		StopLastMusic = playit;

		if ( cur==0 )
		{
			if( !playit
			OR  GetMusic()!=(PtrTrackCD[num]&MUSIC) )
			{
				StopMusic() ;

				if( PtrTrackCD[num]&JINGLE )
				{
					PlayJingle( PtrTrackCD[num]&MUSIC ) ;
				}
			}
		}
		else
		{
			if( cur!=(PtrTrackCD[num]&MUSIC) )// on n'empile pas sur la meme
			{
				NextMusic = num ;// empile musique à jouer +tard
			}
		}

		NextMusicTimer = TimerRefHR+TEST_MUSIC_TEMPO ;

		UnlockTimer() ;
	}
}

/*──────────────────────────────────────────────────────────────────────────*/

S32	GetMusic(void)
{
	return GetNumJingle(StreamName());
}

/*──────────────────────────────────────────────────────────────────────────*/

void StopMusic(void)
{
	StopStream();
}

/*──────────────────────────────────────────────────────────────────────────*/

void PauseMusic(S32 fade)
{
	SaveTimer() ;

	if (fade) {
		FadeOutVolumeMusic();
	}

	PauseStream() ;

	if (fade)
	{
		SetVolumeJingle(JingleVolume);
	}

	RestoreTimer() ;
}

/*──────────────────────────────────────────────────────────────────────────*/

void ResumeMusic(S32 fade)
{
	SaveTimer() ;

	if (fade)
	{
		SetVolumeJingle( 0 ) ;
	}

	StopMusic() ;
	ResumeStream()  ;

	if (fade)	FadeInVolumeMusic();

	StopLastMusic = FALSE;

	RestoreTimer();
}

/*──────────────────────────────────────────────────────────────────────────*/

#define	FADE_MUSIC_TIME		100	// 1/5s

void FadeOutVolumeMusic(void)
{
	S32	timer ;
	U32	startjvol, jvolume, oldjvol ;

	ManageTime();
	SaveTimer() ;

	timer = TimerRefHR ;

	startjvol = oldjvol = jvolume = GetVolumeStream() ;

	while(jvolume > 0)
	{
		ManageTime();

		// volume des Jingles
		jvolume = BoundRegleTrois( startjvol, 0, FADE_MUSIC_TIME, TimerRefHR-timer ) ;

		if( jvolume!=oldjvol )
		{
			SetVolumeJingle( jvolume ) ;
			oldjvol = jvolume ;
		}
	}

	SetVolumeJingle(0) ;

	RestoreTimer() ;
}

/*──────────────────────────────────────────────────────────────────────────*/

void FadeInVolumeMusic(void)
{
	S32	timer ;
	U32	startjvol, jvolume, oldjvol ;

	ManageTime();
	SaveTimer() ;

	timer = TimerRefHR ;


	startjvol = oldjvol = jvolume = GetVolumeStream() ;

	while(jvolume < JingleVolume)
	{
		ManageTime();

		// volume des Jingles
		jvolume = BoundRegleTrois( startjvol, JingleVolume, FADE_MUSIC_TIME, TimerRefHR-timer ) ;

		if( jvolume!=oldjvol )
		{
			SetVolumeJingle( jvolume ) ;
			oldjvol = jvolume ;
		}
	}

	SetVolumeJingle( JingleVolume ) ;

	RestoreTimer() ;
}
/*──────────────────────────────────────────────────────────────────────────*/

void CheckNextMusic(void)
{
	if (NextMusic != -1)
	{
		if (TimerRefHR > NextMusicTimer)
		{
			PlayMusic(NextMusic, FALSE);
		}
	}
}

