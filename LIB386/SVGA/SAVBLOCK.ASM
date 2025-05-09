;----------------------------------------------------------------------------
			.386P
			.MODEL SMALL, C

;----------------------------------------------------------------------------
			.DATA

;----------------------------------------------------------------------------
			EXTRN	C	TabOffLine:DWORD

			ASSUME DS:SEG TabOffLine

;----------------------------------------------------------------------------
			.CODE

;----------------------------------------------------------------------------
			PUBLIC	C	SaveBlock

;----------------------------------------------------------------------------
;void	SaveBlock( void *screen, void *buffer, S32 x0, S32 y0, S32 x1, S32 y1)

;#pragma aux SaveBlock	"*"				\
;	parm	[esi] [edi] [eax] [ebx] [ecx] [edx]

SaveBlock		proc \
			uses esi edi eax ebx ecx edx ebp,
			screen:DWORD,\
			buffer:DWORD,\
			x0:DWORD, y0:DWORD,\
			x1:DWORD, y1:DWORD,

			mov esi, screen
			mov edi, buffer
			mov eax, x0
			mov ebx, y0
			mov ecx, x1
			mov edx, y1

			sub	ecx, eax		; ECX = DeltaX-1
			sub	edx, ebx		; EDX = DeltaY-1

			inc	ecx			; ECX = DeltaX
			add	edx, 2			; EDX = DeltaY+1

			mov	ebx, TabOffLine[ebx*4]
			add	esi, eax

			mov	eax, TabOffLine[1*4]
			add	esi, ebx

			push	ebp
			sub	eax, ecx

			mov	ebx, ecx		; EBX = DeltaX
			mov	ebp, ecx

			shr	ebx, 2
			and	ebp, 11b

			shr	edx, 1
			jnc	odd
Again:
			mov	ecx, ebx		; Delta X
			rep	movsd
			mov	ecx, ebp
			rep	movsb

			add	esi, eax
odd:
			mov	ecx, ebx		; Delta X
			rep	movsd
			mov	ecx, ebp
			rep	movsb

			add	esi, eax

			dec	edx
			jnz	Again

			pop	ebp

			ret

SaveBlock		endp

;----------------------------------------------------------------------------
;			The
       			End
