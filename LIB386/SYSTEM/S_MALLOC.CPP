//──────────────────────────────────────────────────────────────────────────
#include	<system\adeline.h>
#include	<system\logprint.h>
#include	<system\n_malloc.h>
#include	<system\s_malloc.h>

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

//──────────────────────────────────────────────────────────────────────────
static	char	MallocErrorString[256]	;
static	char	MallocErrorFile[260]	;
static	U32	MallocErrorLine		;

//──────────────────────────────────────────────────────────────────────────
void	SafeErrorMallocMsg()
{
	if(MallocErrorString[0])
	{
		LogPrintf("\nMalloc Error: %s in %s at line %d\n", MallocErrorString, MallocErrorFile, MallocErrorLine);
	}
}

//──────────────────────────────────────────────────────────────────────────
static	void	SafeErrorMalloc(char *msg, char *file, U32 line)
{
	strcpy(MallocErrorString, msg);
	strcpy(MallocErrorFile, file);
	MallocErrorLine = line;
	exit(1);
}

//──────────────────────────────────────────────────────────────────────────
static	void	CheckPtr(void *ptr, char *file, U32 line)
{
	if((S32)ptr&(MALLOC_ALIGN-1))
	{
error:		SafeErrorMalloc("Trying to Free or Mshrink a block not allocated with Malloc/SMartMalloc/DosMalloc", file, line);
		return 	;
	}
	switch(*((U8*)ptr-2))
	{
	case 0:
		if(*((U8*)ptr-1)>MALLOC_ALIGN)
		{
			goto error	;
		}
		break;
#ifndef	YAZ_WIN32
	case 1:
		if(*((U8*)ptr-1))
		{
			goto error	;
		}
		break;
#endif
	default:
		goto error	;
	}
}

//──────────────────────────────────────────────────────────────────────────
void	SafeFree( void *ptr, char *file, U32 line )
{
	if(!ptr)
	{
		SafeErrorMalloc("Trying to Free a NULL ptr", file, line);
	}

	CheckPtr(ptr, file, line);	// check if valid block

	NormFree( ptr ) 	;
}
//──────────────────────────────────────────────────────────────────────────
void	*SafeMshrink( void *ptr, U32 taille, char *file, U32 line )
{
	if(!ptr)
	{
		SafeErrorMalloc("Trying to Mshrink a NULL ptr", file, line);
	}

	CheckPtr(ptr, file, line);	// check if valid block

	ptr = NormMshrink(ptr, taille);

	return ptr	;
}

//──────────────────────────────────────────────────────────────────────────

