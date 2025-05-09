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

		public C	LongInverseRotatePointF

;*══════════════════════════════════════════════════════════════════════════*
;void	LongInverseRotatePointF(S32 *Mat, S32 x, S32 y, S32 z)	;

;#pragma aux LongInverseRotatePointF			\
;	parm caller	[esi] [eax] [ebx] [ecx]	\
;	modify exact	[]

LongInverseRotatePointF proc

		mov	[X0], eax
		mov	[Y0], ebx
		mov	[Z0], ecx
					; 0  1  2  3  4  5  6  7

		fild	[X0]		; X

		fild	[Y0]		; Y  X

		fild	[Z0]		; Z  Y  X

		fld	st(2)		; X  Z  Y  X

		fmul	[esi].Struc_MatriceMAT.MAT_M11	; X1 Z  Y  X

		fld	st(2)		; Y  X1 Z  Y  X

		fmul	[esi].Struc_MatriceMAT.MAT_M21	; Y1 X1 Z  Y  X

		fld	st(2)		; Z  Y1 X1 Z  Y  X

		fmul	[esi].Struc_MatriceMAT.MAT_M31	; Z1 Y1 X1 Z  Y  X

		fld	st(5)		; X  Z1 Y1 X1 Z  Y  X

		fmul	[esi].Struc_MatriceMAT.MAT_M12   ; X2 Z1 Y1 X1 Z  Y  X

		fld	st(5)		; Y  X2 Z1 Y1 X1 Z  Y  X

		fmul	[esi].Struc_MatriceMAT.MAT_M22   ; Y2 X2 Z1 Y1 X1 Z  Y  X
		fxch	st(3)		; Y1 X2 Z1 Y2 X1 Z  Y  X

		faddp	st(4), st(0)    ; X2 Z1 Y2 P1 Z  Y  X
		fxch	st(6)		; X  Z1 Y2 P1 Z  Y  X2

		fmul	[esi].Struc_MatriceMAT.MAT_M13	; X3 Z1 Y2 P1 Z  Y  X2
		fxch	st(6)		; X2 Z1 Y2 P1 Z  Y  X3

		fld	st(4)		; Z  X2 Z1 Y2 P1 Z  Y  X3

		fmul	[esi].Struc_MatriceMAT.MAT_M32	; Z2 X2 Z1 Y2 P1 Z  Y  X3
		fxch	st(1)		; X2 Z2 Z1 Y2 P1 Z  Y  X3

		faddp	st(3), st(0)	; Z2 Z1 P2 P1 Z  Y  X3
		fxch	st(5)		; Y  Z1 P2 P1 Z  Z2 X3

		fmul	[esi].Struc_MatriceMAT.MAT_M23	; Y3 Z1 P2 P1 Z  Z2 X3
		fxch	st(1)		; Z1 Y3 P2 P1 Z  Z2 X3

		faddp	st(3), st(0)	; Y3 P2 1  Z  Z2 X3
		fxch	st(3)		; Z  P2 1  Y3 Z2 X3

		fmul	[esi].Struc_MatriceMAT.MAT_M33	; Z3 P2 1  Y3 Z2 X3
		fxch	st(3)		; Y3 P2 1  Z3 Z2 X3

		faddp	st(5), st(0)	; P2 1  Z3 Z2 P3

		faddp	st(3), st(0)	; 1  Z3 2  P3

; with translation

;		fadd	[esi].Struc_MatriceMAT.MAT_TX	; T1 Z3 2  P3
;		fxch	st(3)		; P3 Z3 2  T1

;		faddp	st(1), st(0)	; 3  2  T1
;		fxch	st(1)		; 2  3  T1

;		fadd	[esi].Struc_MatriceMAT.MAT_TY	; T2 3  T1
;		fxch	st(2)		; T1 3  T2

;		fistp	[X0]		; 3  T2

;		faddp	[esi].Struc_MatriceMAT.MAT_TZ	; T3 T2
;		fxch	st(1)		; T2 T3

; 		fistp	[Y0]		; T3

;		fistp	[Z0]

;		ret

; without translation

		fistp	[X0]		; Z3 2  P3

		faddp	st(2), st(0)	; 2  3

		fistp	[Y0]

		fistp	[Z0]

		ret

LongInverseRotatePointF endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		END
