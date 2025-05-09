;*══════════════════════════════════════════════════════════════════════════*

		.386p

		.model SMALL, C

;*══════════════════════════════════════════════════════════════════════════*

		.DATA

		extrn	C	SinTab:WORD
		extrn	C	CosTab:WORD

		extrn C	X0:DWORD
		extrn C	Z0:DWORD

		ASSUME  DS:SEG SinTab

;*══════════════════════════════════════════════════════════════════════════*

		.CODE

		public	C	LongRotateI

;*══════════════════════════════════════════════════════════════════════════*
;void	LongRotateI(S32 x, S32 z, S32 angle)	;

;#pragma	aux	LongRotateI		\
;	parm		[eax] [ecx] [edx]	\
;	modify		[ebx esi edi]

LongRotateI	proc

		and	edx, 4095
		jz	norot

		add	edx, edx
		push	ebp

		movsx	ebp, SinTab[edx]	; ebp = Sin( T )
		movsx	esi, CosTab[edx]	; esi = Cos( T )

		push	ecx
		mov	ebx, eax

		imul	esi

		mov	edi, eax		; X*cos(T)
		mov	eax, [esp]

		mov	ecx, edx

		imul	ebp			; Z*sin(T)

		add	edi, eax
		pop	eax

		adc	ecx, edx

		shrd	edi, ecx, 14		; edi= X'= X*cos(T)+Z*sin(T)

		adc	edi, 0

		imul	esi			; Z*cos(T)

		mov	ecx, eax
		mov	esi, edx

		mov	eax, ebx

		imul	ebp			; X*sin(T)

		sub	ecx, eax
		mov	[X0], edi		; X'

		sbb	esi, edx		; Z' = -X*sin(T) + Z*cos(T)

		shrd	ecx, esi, 14		; ecx = Z'

		adc	ecx, 0
		pop	ebp

		mov	[Z0], ecx		; Z'

		ret
norot:
		mov	[X0], eax		; X'
		mov	[Z0], ecx		; Z'

		ret

LongRotateI	endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		END

