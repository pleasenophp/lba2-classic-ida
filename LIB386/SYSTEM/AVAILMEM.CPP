//──────────────────────────────────────────────────────────────────────────
#include	<system\adeline.h>
#include	<system\availmem.h>

//──────────────────────────────────────────────────────────────────────────
#ifdef	YAZ_WIN32

U32	AvailableMem()
{
	MEMORYSTATUS	mem				;

	GlobalMemoryStatus(&mem)			;

	return mem.dwAvailPageFile+mem.dwAvailPhys	;
}


//──────────────────────────────────────────────────────────────────────────
#else// YAZ_WIN32

typedef	struct
	{
                U32     LargestBlockAvail       ;
                U32     MaxUnlockedPage         ;
                U32     LargestLockablePage     ;
                U32     LinAddrSpace            ;
                U32     NumFreePagesAvail       ;
                U32     NumPhysicalPagesFree    ;
                U32     TotalPhysicalPages      ;
                U32     FreeLinAddrSpace        ;
                U32     SizeOfPageFile          ;
                U32     Reserved[3]             ;
	}	MEMINFO	;

U32	GetMemInfo(MEMINFO *buffer)	;

#pragma aux	GetMemInfo	=	\
"	mov	eax, 0500h	"	\
"	push	ebp		"	\
"	int	31h		"	\
"	pop	ebp		"	\
	parm	[edi]			\
	modify 	[eax ebx ecx edx esi]

U32	AvailableMem()
{
	MEMINFO	meminfo			;

	GetMemInfo(&meminfo)		;

	return meminfo.LargestBlockAvail;
}

//──────────────────────────────────────────────────────────────────────────
#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
