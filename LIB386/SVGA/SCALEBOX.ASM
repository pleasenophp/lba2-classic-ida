;*══════════════════════════════════════════════════════════════════════════*/
		.386p
		.model FLAT, C

;*══════════════════════════════════════════════════════════════════════════*/
		.DATA

;*══════════════════════════════════════════════════════════════════════════*/
		extrn	C 	Log		:DWORD
		extrn	C 	ModeDesiredX	:DWORD
		extrn	C 	TabOffLine	:DWORD

;*══════════════════════════════════════════════════════════════════════════*/
PoidFort	dd	0000
AccuVirguleH	dd	0000
VirguleH	dd	0000
PoidFortH	dd	0000
OffsetLine	dd	0000
Largeur 	dd	0000
Hauteur 	dd	0000

;*══════════════════════════════════════════════════════════════════════════*/
		.CODE

;*══════════════════════════════════════════════════════════════════════════*/
		public   C     ScaleBox

;*══════════════════════════════════════════════════════════════════════════*/
;void	ScaleBox(	S32 xs0, S32 ys0, S32 xs1, S32 ys1, void *ptrs,
;			S32 xd0, S32 yd0, S32 xd1, S32 yd1, void *ptrd	)	;

;#pragma aux	ScaleBox	"*"	\
;	parm	[edx] [ecx] [eax] [edi] [esi] [ebx]

ScaleBox	proc	\
		uses eax ebx ecx edx edi esi ebp,\
		xs0:DWORD, ys0:DWORD, xs1:DWORD, ys1:DWORD, ptrs:DWORD, xd0:DWORD, \
		yd0:DWORD, xd1:DWORD, yd1:DWORD, ptrd:DWORD

		mov edx, xs0
		mov ecx, ys0
		mov eax, xs1
		mov edi, ys1
		mov esi, ptrs
		mov ebx, xd0

		push	edi			; ys1
		push	ecx			; ys0

		mov	edi, ptrd
		mov	ecx, xd1

		sub	eax, edx		; delta source
		add	esi, edx

		sub	ecx, ebx		; ECX delta x screen
		add	edi, ebx

		inc	eax
		inc	ecx

		shl	eax, 16 		; *65536
		xor	edx, edx

		div	ecx

		mov	[Largeur], ecx
		mov	edx, eax

		shl	eax, 16
		mov	ebx, [ModeDesiredX]

		shr	edx, 16
		sub	ebx, ecx

		mov	[OffsetLine], ebx
		mov	ebx, eax

		mov	ecx, yd1
		mov	[PoidFort], edx

		pop	edx
		mov	eax, yd0

		sub	ecx, eax
		add	edi, TabOffLine[eax*4]

		pop	eax
		add	esi, TabOffLine[edx*4]

		sub	eax, edx
		xor	edx, edx

		inc	ecx
		inc	eax

		shl	eax, 16
		push	ebp

		div	ecx

		mov	[Hauteur], ecx
		mov	edx, eax

		shl	eax, 16
		mov	ebp, esi

		shr	edx, 16
		mov	[VirguleH], eax

		mov	[PoidFortH], edx
		xor	edx, edx

		mov	[AccuVirguleH], edx

yloop:		mov	ecx, [Largeur]
		xor	edx, edx

xloop:		mov	al, byte ptr[esi]
		add	edx, ebx		; virgule 16 bits

		mov	[edi], al
		mov	eax, [PoidFort]

		adc	esi, eax	 	; poid fort
		inc	edi

		dec	ecx
		jnz	xloop

		xor	eax, eax
		mov	edx, [AccuVirguleH]

		mov	esi, [VirguleH]
		add	edx, esi	 	; virgule 16 bits

		mov	esi, [PoidFortH]
		mov	[AccuVirguleH], edx

		adc	eax, esi		; poid fort
		mov	esi, [OffsetLine]

		add	edi, esi		; width-delta x screen
		mov	edx, [Hauteur]

		mov	esi, ebp
		mov	eax, TabOffLine[eax*4]

		add	ebp, eax
		dec	edx

		mov	[Hauteur], edx
		jnz	yloop

		pop	ebp

		ret

ScaleBox	endp

;*══════════════════════════════════════════════════════════════════════════*/
;		The
		End
