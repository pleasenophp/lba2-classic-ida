;*══════════════════════════════════════════════════════════════════════════*

		.386p

		.model SMALL, C

		include mat.inc

;*══════════════════════════════════════════════════════════════════════════*

		.DATA

		extrn C	X0:DWORD
		extrn C	Y0:DWORD
		extrn C	Z0:DWORD

;*══════════════════════════════════════════════════════════════════════════*

		.CODE

		public C	LongRotateListF

;*══════════════════════════════════════════════════════════════════════════*
;void	LongRotateListF(MTYPE_MAT *Mat, S32 *dst, S32 *src, S32 n)	;

;#pragma aux LongRotateListF			\
;	parm 		[ebx] [edi] [esi] [ecx]	\
;	modify exact	[ecx esi edi]

LongRotateListF proc
		lea	ecx, [ecx+ecx*2]

		test	ecx, ecx
		jz	Exit

		lea	esi, [esi+ecx*4]
		lea	ecx, [ecx*4]

		add	edi, ecx
		xor	ecx, -1

		inc	ecx
		jmp	EnterLoop
					;  0   1   2   3   4   5   6   7
NextPoint:
		faddp	st(3), st(0)	; 1  Z3 2  P3

		fistp	dword ptr[edi+ecx-12]; Z3 2  P3

		faddp	st(2), st(0)	; 2  3

		fistp	dword ptr[edi+ecx-8]; 3

		fistp	dword ptr[edi+ecx-4]

EnterLoop:
		fild	dword ptr[esi+ecx];X

		fild	dword ptr[esi+ecx+4];Y   X

		fild	dword ptr[esi+ecx+8];Z   Y   X

		fld	st(2)		; X  Z  Y  X

		fmul	[ebx].Struc_MatriceMAT.MAT_M11	; X1 Z  Y  X

		fld	st(2)		; Y  X1 Z  Y  X

		fmul	[ebx].Struc_MatriceMAT.MAT_M12	; Y1 X1 Z  Y  X

		fld	st(2)		; Z  Y1 X1 Z  Y  X

		fmul	[ebx].Struc_MatriceMAT.MAT_M13	; Z1 Y1 X1 Z  Y  X

		fld	st(5)		; X  Z1 Y1 X1 Z  Y  X

		fmul	[ebx].Struc_MatriceMAT.MAT_M21   ; X2 Z1 Y1 X1 Z  Y  X

		fld	st(5)		; Y  X2 Z1 Y1 X1 Z  Y  X

		fmul	[ebx].Struc_MatriceMAT.MAT_M22   ; Y2 X2 Z1 Y1 X1 Z  Y  X
		fxch	st(3)		; Y1 X2 Z1 Y2 X1 Z  Y  X

		faddp	st(4), st(0)    ; X2 Z1 Y2 X1 Z  Y  X
		fxch	st(6)		; X  Z1 Y2 X1 Z  Y  X2

		fmul	[ebx].Struc_MatriceMAT.MAT_M31	; X3 Z1 Y2 X1 Z  Y  X2
		fxch	st(6)		; X2 Z1 Y2 P1 Z  Y  X3

		fld	st(4)		; Z  X2 Z1 Y2 P1 Z  Y  X3

		fmul	[ebx].Struc_MatriceMAT.MAT_M23	; Z2 X2 Z1 Y2 P1 Z  Y  X3
		fxch	st(1)		; X2 Z2 Z1 Y2 P1 Z  Y  X3

		faddp	st(3), st(0)	; Z2 Z1 P2 P1 Z  Y  X3
		fxch	st(5)		; Y  Z1 P2 P1 Z  Z2 X3

		fmul	[ebx].Struc_MatriceMAT.MAT_M32	; Y3 Z1 P2 P1 Z  Z2 X3
		fxch	st(1)		; Z1 Y3 P2 P1 Z  Z2 X3

		faddp	st(3), st(0)	; Y3 P2 1  Z  Z2 X3
		fxch	st(3)		; Z  P2 1  Y3 Z2 X3

		fmul	[ebx].Struc_MatriceMAT.MAT_M33	; Z3 P2 1  Y3 Z2 X3
		fxch	st(3)		; Y3 P2 1  Z3 Z2 X3

		faddp	st(5), st(0)	; P2 1  Z3 Z2 P3

		add	ecx, 12
		jnz	NextPoint

		faddp	st(3), st(0)	; 1  Z3 2  P3

		fistp	dword ptr[edi-12]; Z3 2  P3

		faddp	st(2), st(0)	; 2  3

		fistp	dword ptr[edi-8]; 3

		fistp	dword ptr[edi-4]
Exit:
		ret

LongRotateListF endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		END
