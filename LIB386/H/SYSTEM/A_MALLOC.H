//──────────────────────────────────────────────────────────────────────────
#ifndef	LIB_SYSTEM_A_MALLOC
#define LIB_SYSTEM_A_MALLOC

//──────────────────────────────────────────────────────────────────────────
#include	<system\s_malloc.h>
#include	<system\n_malloc.h>

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
extern	"C"	{
#endif

//──────────────────────────────────────────────────────────────────────────
#define Malloc			NormMalloc
#define DosMalloc		NormDosMalloc
#define DosFree 		NormDosFree

//──────────────────────────────────────────────────────────────────────────
#ifdef  DEBUG_MALLOC

#define Mshrink(a, b)           SafeMshrink(a, b, __FILE__, __LINE__)
#define Free(a)                 SafeFree(a, __FILE__, __LINE__)

//──────────────────────────────────────────────────────────────────────────
#else// DEBUG_MALLOC

#define Mshrink(a, b)           NormMshrink(a, b)
#define Free(a)                 NormFree(a)

#endif//DEBUG_MALLOC

//──────────────────────────────────────────────────────────────────────────
#ifdef	YAZ_WIN32

#define	SmartMalloc(size)	Malloc(size)

#else// YAZ_WIN32

extern	void	*SmartMalloc(S32 size)		;

#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
}
#endif

//──────────────────────────────────────────────────────────────────────────
#endif//LIB_SYSTEM_N_MALLOC

//──────────────────────────────────────────────────────────────────────────
