//──────────────────────────────────────────────────────────────────────────
#include "TIMERWIN.H"

#ifndef	_WIN32

//──────────────────────────────────────────────────────────────────────────
#ifndef	LIB_SYSTEM_TIMERDOS
#define LIB_SYSTEM_TIMERDOS

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
extern	"C"	{
#endif

//──────────────────────────────────────────────────────────────────────────
extern	volatile U32	TimerSystemHR	;
extern	volatile U32	TimerRefHR	;

//──────────────────────────────────────────────────────────────────────────
extern	volatile S32	TimerLock	;

//──────────────────────────────────────────────────────────────────────────
extern	volatile S32	CmptFrame	;
extern	volatile S32	NbFramePerSecond;

//──────────────────────────────────────────────────────────────────────────
extern	volatile S32	CmptMemoTimerRef;

//──────────────────────────────────────────────────────────────────────────
extern	void		NewProc08(U32 user) ;

#pragma aux NewProc08	"*"	\
	parm	[]		\
	modify	[]

//──────────────────────────────────────────────────────────────────────────
/*
void	SetDivisor(U32 counter)	;

#pragma aux SetDivisor	"*"	\
	parm caller	[ecx]	\
	modify	[eax]
*/
//──────────────────────────────────────────────────────────────────────────
//#define	ManageTime()

extern void ManageTime();

//──────────────────────────────────────────────────────────────────────────
extern	void	SaveTimer()	;

#pragma aux SaveTimer	"*"	\
	modify	[eax ecx edx]

//──────────────────────────────────────────────────────────────────────────
extern	void	RestoreTimer()	;

#pragma aux RestoreTimer "*"	\
	modify	[eax edx]

//──────────────────────────────────────────────────────────────────────────
#define	LockTimer()	TimerLock++

//──────────────────────────────────────────────────────────────────────────
#define	UnlockTimer()	{ if(TimerLock)	TimerLock--; }

//──────────────────────────────────────────────────────────────────────────
void	SetTimerHR(U32 timer)	;

#pragma aux SetTimerHR	"*"	\
	parm caller	[eax]	\
	modify	[ecx edx]

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
}
#endif

//──────────────────────────────────────────────────────────────────────────
#endif//LIB_SYSTEM_TIMERDOS

//──────────────────────────────────────────────────────────────────────────
#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
