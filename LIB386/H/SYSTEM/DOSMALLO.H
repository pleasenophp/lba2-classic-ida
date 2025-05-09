//──────────────────────────────────────────────────────────────────────────
#ifndef	LIB_SYSTEM_DOSMALLOC
#define LIB_SYSTEM_DOSMALLOC

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
extern	"C"	{
#endif

//──────────────────────────────────────────────────────────────────────────
#ifdef	WIN32

#define	NormDosMalloc	NormMalloc

#else //YAZ_WIN32

extern	void	*NormDosMalloc(S32 size)	;

#pragma aux	NormDosMalloc	"*"		\
	parm caller	[ebx]			\
	modify		[eax ecx edx esi edi]

#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
#ifdef	YAZ_WIN32

#define	NormDosFree	NormFree

#else //YAZ_WIN32

extern	void	NormDosFree(void *ptr)		;

#pragma aux	NormDosFree	"*"		\
	parm caller	[edx]	  		\
	modify 		[eax ebx ecx esi edi]


#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
#ifdef	YAZ_WIN32

#define	NormDosMshrink	NormMshrink

#else //YAZ_WIN32

void	*NormDosMshrink(void *ptr, U32 size)	;

#pragma aux	NormDosMshrink	"*"	\
	parm	[edx] [ebx]	  	\
	modify 	[eax ecx esi edi]

#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
#ifdef	YAZ_WIN32

#define	DosAvailableMem()	0

#else //YAZ_WIN32

extern	U32	DosAvailableMem()		;

#pragma aux	DosAvailableMem	"*"		\
	modify 		[eax ebx ecx edx esi edi]


#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
}
#endif

//──────────────────────────────────────────────────────────────────────────
#endif//LIB_SYSTEM_DOSMALLOC

//──────────────────────────────────────────────────────────────────────────
