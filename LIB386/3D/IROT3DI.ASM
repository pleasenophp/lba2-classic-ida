;*══════════════════════════════════════════════════════════════════════════*

		.386p

		.model SMALL, C

		include mat.inc

;*══════════════════════════════════════════════════════════════════════════*

		.DATA

		extrn C	X0:DWORD
		extrn C	Y0:DWORD
		extrn C	Z0:DWORD
		ASSUME ds:SEG X0

;*══════════════════════════════════════════════════════════════════════════*

		.CODE

		public C	InverseRotatePointI

;*══════════════════════════════════════════════════════════════════════════*
;void	InverseRotatePointI(S32 *Mat, S32 x, S32 y, S32 z)	;

;#pragma aux InverseRotatePointI			\
;	parm caller	[esi] [eax] [ebx] [ecx]	\
;	modify exact	[eax ebx ecx edx edi]

InverseRotatePointI	proc

		push	ebp
		mov	edi, [esi].Struc_MatriceMAT.MAT_M11

		mov	ebp, [esi].Struc_MatriceMAT.MAT_M21

		imul	edi, eax
		imul	ebp, ebx

		mov	edx, [esi].Struc_MatriceMAT.MAT_M31
		add	edi, ebp

		imul	edx, ecx

		mov	ebp, [esi].Struc_MatriceMAT.MAT_M22
		lea	edx, [edx+edi+(1 SHL 13)]

		sar	edx, 14
		mov	edi, [esi].Struc_MatriceMAT.MAT_M12

		mov	X0, edx
		mov	edx, [esi].Struc_MatriceMAT.MAT_M32

		imul	ebp, ebx
		imul	edi, eax
		imul	edx, ecx
		imul	eax, [esi].Struc_MatriceMAT.MAT_M13
		imul	ebx, [esi].Struc_MatriceMAT.MAT_M23
		imul	ecx, [esi].Struc_MatriceMAT.MAT_M33

		add	edx, ebp
		add	ebx, eax

		lea	edx, [edx+edi+1 SHL 13]
		lea	ebx, [ebx+ecx+1 SHL 13]

		sar	edx, 14
		pop	ebp

		sar	ebx, 14
		mov	Y0, edx

		mov	Z0, ebx

		; X0=X' Y0=Y' Z0=Z'

		ret

InverseRotatePointI	endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		End
