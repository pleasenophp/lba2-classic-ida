#include	<system\adeline.h>

#ifdef	YAZ_WIN32
#include	<system\winsys.h>
#else
#include	<system\dossys.h>
#endif

#include	<ail\common.h>
#include	<ail\sample.h>
#include	<ail\stream.h>

#pragma pack(8)
#include	"soloud.h"
#include	"soloud_wav.h"
#pragma pack(1)

#include	<assert.h>

#include 	<stdlib.h>
#include 	<string.h>

#include	<stdio.h>

extern	S32		LibWaveMasterVolume	;

//████████████████████████████████████████████████████████████████████████████

static	S32		StreamVolume = 127;
static	S32		PausedPos;
static	char	StreamPathName[_MAX_PATH];
static	char	PausedPathName[_MAX_PATH];

//████████████████████████████████████████████████████████████████████████████

static	VOID_FUNC	*StreamManageProc;

//████████████████████████████████████████████████████████████████████████████

static	VOID_FUNC	ManageStream;

//████████████████████████████████████████████████████████████████████████████

U32 soloudHandle = 0;
SoLoud::Wav* soloudWave = nullptr;
SoLoud::Soloud* soloudStream = NULL;


static void	ManageStream()
{
	if (StreamManageProc)
	{
		StreamManageProc();
	}
}

//████████████████████████████████████████████████████████████████████████████

static void PlayThisStream(char *name, S32 pos)
{
	if (!Sample_Driver_Enabled) return;

	StopStream()	;

	strcpy(StreamPathName, name)		;

	soloudWave = new SoLoud::Wav();
	soloudWave->load(StreamPathName);
	soloudHandle = soloudStream->play(*soloudWave, StreamVolume / 127.0f, 0, true, 0);
	soloudStream->setAutoStop(soloudHandle, true);
	soloudStream->setPause(soloudHandle, false);

	ManageStream()				;
}

//████████████████████████████████████████████████████████████████████████████

void OpenStream()
{
	static	char init = TRUE;

	if(init)
	{
		init 		 = FALSE;

		// xesf
		S32 rate = 44100;
		S32 chans = 2;
		soloudStream = new SoLoud::Soloud();
		soloudStream->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::BACKENDS::AUTO, rate, 0, chans);
	}
}

//████████████████████████████████████████████████████████████████████████████

void PlayStream(char *name)
{
	PlayThisStream(name, 0)	;
}

//████████████████████████████████████████████████████████████████████████████

void ChangeVolumeStream(S32 volume)
{
	if (!Sample_Driver_Enabled) return;

	if (volume > 127)
	{
		volume = 127;
	}
	else if (volume < 0)
	{
		volume = 0;
	}

	StreamVolume = volume;

	if (!soloudStream || !soloudHandle) return;

	soloudStream->setVolume(soloudHandle, volume / 127.0f);
}

//████████████████████████████████████████████████████████████████████████████

S32	GetVolumeStream()
{
	return StreamVolume;
}

//████████████████████████████████████████████████████████████████████████████

void StopStream()
{
	soloudStream->stop(soloudHandle);
	delete soloudWave;
	soloudWave = nullptr;
	soloudHandle = 0;
	StreamPathName[0] = 0;
}

//████████████████████████████████████████████████████████████████████████████

void PauseStream()
{
	if (!soloudStream) return;
	if (!IsStreamPlaying())	return;

	soloudStream->setPause(soloudHandle, true);
	ManageStream()				;
	strcpy(PausedPathName, StreamPathName);
}

//████████████████████████████████████████████████████████████████████████████

void ResumeStream()
{
	if (!soloudStream) return;

	if (!PausedPathName[0]) return;

	soloudStream->setPause(soloudHandle, false);

	PausedPathName[0] = 0;
}

//████████████████████████████████████████████████████████████████████████████

S32	IsStreamPlaying()
{
	S32	playing = false	;

	if (!Sample_Driver_Enabled || !soloudWave || !soloudStream) return FALSE;

	ManageStream()			;

	playing = soloudStream->isValidVoiceHandle(soloudHandle);

	if (!playing) StopStream();

	return playing;
}

//████████████████████████████████████████████████████████████████████████████

char	*StreamName()
{
	IsStreamPlaying()	;

	return	StreamPathName	;
}

//████████████████████████████████████████████████████████████████████████████
