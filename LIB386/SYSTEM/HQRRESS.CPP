/*──────────────────────────────────────────────────────────────────────────*/
#include	<system\adeline.h>
#include	<system\initimer.h>
#include	<system\a_malloc.h>
#include	<system\lz.h>
#include	<system\files.h>
#include	<system\hqfile.h>
#include	<system\hqrress.h>

#include	<string.h>

//──────────────────────────────────────────────────────────────────────────
typedef struct  {       S32		Index	;
                        void		*Ptr	;
                        S32		Size	;
                        U32		Time	;
		}       T_HQR_BLOC 		;

//──────────────────────────────────────────────────────────────────────────
HQR_GET_CALLBACK	*HQRGetErrorFunc= NULL	;
HQR_GET_CALLBACK	*HQRGetDelFunc	= NULL	;

//──────────────────────────────────────────────────────────────────────────
S32	HQR_Flag ;	// flag "load done" par HQR_Get

//──────────────────────────────────────────────────────────────────────────
// initialise la gestion bufferisée d'une ressource
// hqrname: nom du .HQR contenant les fiches compactées
// maxsize: taille buffer memoire
// MaxIndex: nombre de fiche max stocké
// (devrait être > maxsize/average file size )
T_HQR_HEADER	*HQR_Init_Ressource(	char	*hqrname,
 					S32	maxsize,
					S32	maxrsrc	)
{
	T_HQR_HEADER	*header ;
	void		*buffer ;

	header = (T_HQR_HEADER*)Malloc(	sizeof(T_HQR_HEADER)		+
					sizeof(T_HQR_BLOC) * maxrsrc	) ;

	if(!header)
	{
		return NULL	;
	}

	buffer = Malloc(maxsize+RECOVER_AREA);
	if(!buffer)
	{
error:		Free(header)	;
		return NULL	;
	}

	header->MaxSize	= maxsize	;
	header->MaxIndex= maxrsrc	;
	header->Buffer	= buffer	;

	if(!HQR_Change_Ressource(header, hqrname))
	{
		Free(buffer)	;
		goto error	;
	}

	return header		;// header
}

//──────────────────────────────────────────────────────────────────────────
// lock le buffer
#ifndef	YAZ_WIN32
void	HQR_Lock_Ressource(T_HQR_HEADER *header)
{
	header = header	;

	/*AIL_vmm_lock((void*)header, sizeof(T_HQR_HEADER) +
				sizeof(T_HQR_BLOC) * header->MaxIndex   ) ;

	AIL_vmm_lock(header->Buffer, header->MaxSize+RECOVER_AREA);*/
}

#endif//YAZ_WIN32

//──────────────────────────────────────────────────────────────────────────
// vide le buffer d'une ressource
void	HQR_Reset_Ressource(T_HQR_HEADER *header)
{
	header->FreeSize= header->MaxSize	;
	header->NbIndex	= 0			;
}

//──────────────────────────────────────────────────────────────────────────
// change le nom du fichiers HQR qui contient les fiches (vide le buffer)
S32	HQR_Change_Ressource(T_HQR_HEADER *header, char *newhqrname)
{
	if(newhqrname && newhqrname[0])
	{
		strcpy(header->Name, newhqrname);

		if(!FileSize(newhqrname))
		{
			return FALSE	;
		}

	}
	else
	{
		header->Name[0] = 0	;
	}

	HQR_Reset_Ressource(header)	;

	return TRUE 			;
}

//──────────────────────────────────────────────────────────────────────────
// libère la mémoire allouée à la gestion bufferisée d'une ressource HQR
void	HQR_Free_Ressource(T_HQR_HEADER *header)
{
	if(!header)
	{
		return		;
	}

	/*AIL_vmm_unlock((void*)header, sizeof(T_HQR_HEADER) +
				  sizeof(T_HQR_BLOC) * header->MaxIndex ) ;

	AIL_vmm_unlock(header->Buffer, header->MaxSize+RECOVER_AREA);
	*/
	Free(header->Buffer)	;
	Free(header)		;
}

//──────────────────────────────────────────────────────────────────────────
// retourne le pointeur mémoire de la fiche (index) demandée
T_HQR_BLOC	*HQR_GiveBloc(S32 index, S32 nbindex, T_HQR_BLOC *bloc)
{
	T_HQR_BLOC* returnValue;

	__asm
	{
		pusha

			mov edx, index
			mov ecx, nbindex
			mov eax, bloc
		               shl     ecx, 4          
			               jz      notfound        
			               lea     ebx, [eax+ecx]  
			               sub     eax, 16         
			               test    ecx, 16         
			               jnz     se1             
			               add     eax, 16         
			se0:           cmp     edx, [eax]      
			               je      quit            
			se1:           cmp     edx, [eax+16]   
			               je      quit2           
			               add     eax, 32         
			               cmp     eax, ebx        
			               jne     se0             
			notfound:      xor     eax, eax        
			               jmp     quit            
			quit2:         add     eax, 16         
quit:             
						   mov returnValue, eax
							   popa
	}

	return returnValue;
	/*#pragma aux HQR_GiveBloc       =               	\
	"               shl     ecx, 4          "       \
	"               jz      notfound        "       \
	"               lea     ebx, [eax+ecx]  "       \
	"               sub     eax, 16         "       \
	"               test    ecx, 16         "       \
	"               jnz     se1             "       \
	"               add     eax, 16         "       \
	"se0:           cmp     edx, [eax]      "       \
	"               je      quit            "       \
	"se1:           cmp     edx, [eax+16]   "       \
	"               je      quit2           "       \
	"               add     eax, 32         "       \
	"               cmp     eax, ebx        "       \
	"               jne     se0             "       \
	"notfound:      xor     eax, eax        "       \
	"               jmp     quit            "       \
	"quit2:         add     eax, 16         "       \
	"quit:                                  "       \
	parm    [edx] [ecx] [eax]               \
	modify	[ebx]
*/
}

//──────────────────────────────────────────────────────────────────────────
// supprime une fiche dans le buffer d'une ressource
static inline void HQR_Del_Bloc(T_HQR_HEADER *header, S32 index)
{
	S32		n			;
	T_HQR_BLOC	*ptrbloc		;
	S32		delsize			;
	void		*ptrs, *ptrd		;

	ptrbloc = (T_HQR_BLOC*)(header+1);
	delsize = ptrbloc[index].Size 	;

	// if this is last index then skip this...
	if(index < header->NbIndex-1)
	{
		// shift buffer
		ptrd = ptrbloc[index].Ptr 		;
		ptrs = (void*)((U8*)ptrd + delsize)	;
		memmove(ptrd, ptrs, (header->MaxSize-header->FreeSize)-((U32)ptrs-(U32)(header->Buffer)) ) ;

		// shift index table
		ptrd = (void*)&ptrbloc[index]		;
		ptrs = (void*)&ptrbloc[index+1]		;
		memmove(ptrd, ptrs, (header->NbIndex-index-1)*sizeof(T_HQR_BLOC) ) ;

		// shift index value
		for(n=index; n<(header->NbIndex-1); n++ )
		{
			*(S32*)&(ptrbloc[n].Ptr) -= delsize;
		}
	}

	// update buffer status
	header->NbIndex-- 			;
	header->FreeSize += delsize		;
}

//──────────────────────────────────────────────────────────────────────────
// retourne le pointeur mémoire de la fiche (index) demandée
void	*HQR_Get(T_HQR_HEADER *header, S32 index)
{
	S32		n, oldest	;
	U32		testtime	;
	void		*ptr		;
	T_HQR_BLOC	*ptrbloc	;
	S32		size		;

	if(index < 0) 	goto error	;

	ptrbloc = HQR_GiveBloc(index, header->NbIndex, (T_HQR_BLOC*)(header+1));

	if(ptrbloc)
	{
		// existing index
		ptrbloc->Time	= TimerSystemHR	;// update LRU data

		HQR_Flag	= FALSE		;// NOT NEWLY LOADED

		return	ptrbloc->Ptr 		;
	}
	else	// need load
	{
		// load hqr bloc
		size = HQF_Init(header->Name, index);
		if(!size)
		{
			goto	error		;
		}

		// memory management
		ptrbloc = (T_HQR_BLOC*)(header+1);

		// check if enough space for bloc or index
		while(	(size > header->FreeSize) 		OR
			(header->NbIndex >= header->MaxIndex) 		)
		{
			// delete oldest bloc
			oldest		= -1	;
			testtime	= -1	;

			for( n=0; n<header->NbIndex; n++ )
			{
				if(ptrbloc[n].Time < testtime)
				{
					testtime= ptrbloc[n].Time	;
					oldest	= n 			;
				}
			}
			if(oldest==-1)	// not enough ram or big trouble
			{
				HQF_Close()		;
error:				if(HQRGetErrorFunc)	HQRGetErrorFunc(header->Name, index)	;
				return NULL		;
			}

			if(HQRGetDelFunc)	HQRGetDelFunc(header->Name, index)	;

			HQR_Del_Bloc( header, oldest )	;
		}

		// compute ptr
		ptr = (void *)((U8*)header->Buffer + header->MaxSize - header->FreeSize);

		// space size ok, update struct
		ptrbloc[header->NbIndex].Index	= index		;
		ptrbloc[header->NbIndex].Time	= TimerSystemHR	;
		ptrbloc[header->NbIndex].Ptr	= ptr 		;
		ptrbloc[header->NbIndex].Size	= size		;

		// load it
		if(!HQF_LoadClose(ptr))
		{
			goto error	;
		}

		header->NbIndex++ 	;
		header->FreeSize-= size	;

		HQR_Flag	= TRUE	;// NEWLY LOADED

		return ptr 		;
	}
}

