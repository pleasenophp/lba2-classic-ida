;*══════════════════════════════════════════════════════════════════════════*

		.386p

		.model SMALL, C

		include mat.inc

;*══════════════════════════════════════════════════════════════════════════*

		.DATA

		extrn	C	SinTab:WORD
		extrn	C	CosTab:WORD
		ASSUME ds:SEG CosTab
;*══════════════════════════════════════════════════════════════════════════*

		.CODE

		public	C	InitMatrixStdI

;*══════════════════════════════════════════════════════════════════════════*
;void	InitMatrixStdI(S32 *MatDst, S32 alpha, S32 beta, S32 gamma) ;

;#pragma aux InitMatrixStdI	"*"		\
;	parm		[edi] [eax] [ebx] [ecx]	\
;	modify exact	[eax ebx ecx edx esi]

InitMatrixStdI	proc

		and	ecx, 4095
		and	eax, 4095

		add	ecx, ecx
		add	eax, eax

		and	ebx, 4095
		push	ebp

		add	ebx, ebx

		movsx	ebp, SinTab[ecx]	; ebp = sin(g)
		movsx	esi, SinTab[eax]	; esi = sin(a)

		push	ebx			; Save beta
		mov	edx, ebp

		movsx	ebx, CosTab[ecx]	; ebx = cos(g)
		movsx	ecx, CosTab[eax]	; ecx = cos(a)

		neg	edx
		mov	[edi].Struc_MatriceMAT.MAT_M11, ebx	; cos(g)

		mov	[edi].Struc_MatriceMAT.MAT_M12, edx	; -sin(g)
		mov	edx, ebp

		imul	edx, ecx

		sar	edx, 14

		adc	edx, 0
		mov	eax, ebx

		mov	[edi].Struc_MatriceMAT.MAT_M21, edx	; sin(g)*cos(a)
		mov	edx, ebp

		imul	eax, ecx
		imul	edx, esi

		sar	eax, 14
		lea	edx, [edx + (1 SHL 13)]

		adc	eax, 0
		pop	ebp			; EBP = Beta

		sar	edx, 14
		mov	[edi].Struc_MatriceMAT.MAT_M22, eax	; cos(g)*cos(a)

		mov	[edi].Struc_MatriceMAT.MAT_M31, edx	; sin(g)*sin(a)
		mov	edx, ebx

		imul	edx, esi

		movsx	ebx, CosTab[ebp]	; ebx = cos(b)
		movsx	ebp, SinTab[ebp]	; ebp = sin(b)

		sar	edx, 14
		mov	eax, [edi].Struc_MatriceMAT.MAT_M11	; EAX = Cos(g)

		adc	edx, 0

		mov	[edi].Struc_MatriceMAT.MAT_M32, edx	; cos(g)*sin(a)
		mov	edx, ebx

		imul    edx, eax
		imul	eax, ebp

		sar	edx, 14
		lea	eax, [eax+(1 SHL 13)]

		adc	edx, 0

		sar	eax, 14
		mov	[edi].Struc_MatriceMAT.MAT_M11, edx	; cos(g)*cos(b)

		mov	[edi].Struc_MatriceMAT.MAT_M13, eax	; cos(g)*sin(b)
		mov	eax, [edi].Struc_MatriceMAT.MAT_M21	; EAX = sin(g)*cos(a)

		mov	edx, ebp
		push	eax

		imul	eax, ebx
		imul	edx, esi

		lea	edx, [edx+eax+(1 SHL 13)]

		sar	edx, 14
		pop	eax

		mov	[edi].Struc_MatriceMAT.MAT_M21, edx	; sin(b)*sin(a)+sin(g)*cos(a)*cos(b)
		mov	edx, ebp

		imul	esi, ebx
		imul	edx, eax

		sub	edx, esi
		xor	eax, eax

		sar	edx, 14
		mov	esi, [edi].Struc_MatriceMAT.MAT_M31	; ESI = sin(g)*sin(a)

		adc	edx, 0
		mov	[edi].Struc_MatriceMAT.MAT_MTX, eax

		mov	[edi].Struc_MatriceMAT.MAT_M23, edx	; sin(b)*sin(g)*cos(a)-sin(a)*cos(b)
		mov	edx, ecx

		imul	ecx, ebx		; ca*cb
		imul	edx, ebp		; ca*sb
		imul	ebp, esi		; sb*sin(g)*sin(a)
		imul	esi, ebx		; sin(g)*sin(a)*cb

		add	ebp, ecx
		sub	esi, edx

		sar	ebp, 14
		mov	[edi].Struc_MatriceMAT.MAT_MTY, eax

		adc	ebp, 0
		mov	[edi].Struc_MatriceMAT.MAT_MTZ, eax

		sar	esi, 14
		mov	[edi].Struc_MatriceMAT.MAT_M33, ebp	; sin(b)*sin(g)*sin(a)+cos(a)*cos(b)

		adc	esi, 0
		pop	ebp

		mov	[edi].Struc_MatriceMAT.MAT_M31, esi	; sin(g)*sin(a)*cos(b)-cos(a)*sin(b)

		ret

InitMatrixStdI	endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		END
