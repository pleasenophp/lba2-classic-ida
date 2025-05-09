;*══════════════════════════════════════════════════════════════════════════*

		.386p

		.model SMALL, C

		include mat.inc

;*══════════════════════════════════════════════════════════════════════════*

		.DATA

;*══════════════════════════════════════════════════════════════════════════*
		extrn	C MatriceWorld:DWORD

		extrn	C CameraXr:DWORD
		extrn	C CameraYr:DWORD
		extrn	C CameraZr:DWORD

		extrn	C CameraZrClip:DWORD

		extrn	C NearClip:DWORD

		extrn	C FRatioX:DWORD
		extrn	C FRatioY:DWORD

		extrn	C XCentre:DWORD
		extrn	C YCentre:DWORD

		extrn	C Xp:DWORD
		extrn	C Yp:DWORD

		ASSUME  DS:SEG CameraXr

;*══════════════════════════════════════════════════════════════════════════*

		.CODE

		public C	LongWorldRotateProjectPoint3DF
		public C	LongRotateProjectPoint3DF

;*══════════════════════════════════════════════════════════════════════════*
;void	LongWorldRotateProjectPoint3DF(S32 x, S32 y, S32 z)	;

;#pragma aux LongWorldRotateProjectPoint3DF	\
;	parm caller	[eax] [ebx] [ecx]	\
;	modify		[esi edx]

LongWorldRotateProjectPoint3DF proc

		mov	esi, offset MatriceWorld

;*══════════════════════════════════════════════════════════════════════════*
;void	LongRotateProjectPoint3DF(S32 *Mat, S32 x, S32 y, S32 z)	;

;#pragma aux LongRotateProjectPoint3DF		\
;	parm caller	[esi] [eax] [ebx] [ecx]	\
;	modify		[edx]

LongRotateProjectPoint3DF::

		push	ecx
		mov	[Yp], ebx

		mov	[Xp], eax
					; 0  1  2  3  4  5  6  7

		fild	dword ptr[esp]  ; Z

		fild	[Yp]		; Y  Z

		fild	[Xp]		; X  Y  Z

		fld	st(2)		; Z  X  Y  Z

		fmul	[esi].Struc_MatriceMAT.MAT_M31	; Z1 X  Y  Z

		fld	st(2)		; Y  Z1 X  Y  Z

		fmul	[esi].Struc_MatriceMAT.MAT_M32	; Y1 Z1 X  Y  Z

		fld	st(2)		; X  Y1 Z1 X  Y  Z

		fmul	[esi].Struc_MatriceMAT.MAT_M33	; X1 Y1 Z1 X  Y  Z

		fld	st(5)		; Z  X1 Y1 Z1 X  Y  Z

		fmul	[esi].Struc_MatriceMAT.MAT_M21   ; Z2 X1 Y1 Z1 X  Y  Z

		fld	st(5)		; Y  Z2 X1 Y1 Z1 X  Y  Z

		fmul	[esi].Struc_MatriceMAT.MAT_M22   ; Y2 Z2 X1 Y1 Z1 X  Y  Z
		fxch	st(3)		; Y1 Z2 X1 Y2 Z1 X  Y  Z

		faddp	st(4), st(0)    ; Z2 X1 Y2 P1 X  Y  Z
		fxch	st(6)		; Z  X1 Y2 P1 X  Y  Z2

		fmul	[esi].Struc_MatriceMAT.MAT_M11	; Z3 X1 Y2 P1 X  Y  Z2
		fxch	st(6)		; Z2 X1 Y2 P1 X  Y  Z3

		fld	st(4)		; X  Z2 X1 Y2 P1 X  Y  Z3

		fmul	[esi].Struc_MatriceMAT.MAT_M23	; X2 Z2 X1 Y2 P1 X  Y  Z3
		fxch	st(1)		; Z2 X2 X1 Y2 P1 X  Y  Z3

		faddp	st(3), st(0)	; X2 X1 P2 P1 X  Y  Z3
		fxch	st(5)		; Y  X1 P2 P1 X  X2 Z3

		fmul	[esi].Struc_MatriceMAT.MAT_M12	; Y3 X1 P2 P1 X  X2 Z3

		fild	[CameraZrClip]	; ZC Y3 X1 P2 P1 X  X2 Z3

		fxch	st(2)		; X1 Y3 ZC P2 P1 X  X2 Z3

		faddp	st(4), st(0)	; Y3 ZC P2 Z' X  X2 Z3
		fxch	st(4)		; X  ZC P2 Z' Y3 X2 Z3

		fmul	[esi].Struc_MatriceMAT.MAT_M13	; X3 ZC P2 Z' Y3 X2 Z3
		fxch	st(4)		; Y3 ZC P2 Z' X3 X2 Z3

		faddp	st(6), st(0)	; ZC P2 Z' X3 X2 P3

		fsubrp	st(2), st(0)	; P2 Z" X3 X2 P3

		faddp	st(5), st(0)	; P2 Z" X3 X2 P3

		faddp	st(3), st(0)	; Z" X3 Y' P3

		fst	dword ptr[esp]	; Z" X3 Y' P3

		fxch	st(1)		; X3 Z" Y' P3

		faddp	st(3), st(0)	; Z" Y' X'

		pop	eax

		test	eax, eax
		jl	error

		fild	[NearClip]	; NC Z" Y' X'

		fild	[CameraYr]	; Yr NC Z" Y' X'

		fild	[CameraXr]	; Xr Yr NC Z" Y' X'

		fxch	st(2)		; NC Yr Xr Z" Y' X'

		faddp	st(3), st(0)	; Yr Xr Z  Y' X'

		fsubp	st(3), st(0)	; Xr Z  Y  X'

		fsubp	st(3), st(0)	; Z  Y  X

		fdivr	[FRatioX]	; fx/Z  Y  X

		mov	ecx, [XCentre]
		mov	edx, [YCentre]

		fmul	st(1), st(0)	; fx/Z Y'   X

		; PENALTY 1 cycle on Pentium

		fmulp	st(2), st(0)	; Y'   Xe

		; PENALTY 1 cycle on Pentium

		fmul	[FRatioY]	; Ye   Xe
		fxch	st(1)		; Xe   Ye

		fistp	[Xp]		; Ye

		fistp	[Yp]

		mov	eax, [Xp]
		mov	ebx, [Yp]

		add	eax, ecx
		add	ebx, edx

		mov	[Xp], eax
		mov	[Yp], ebx

		mov	eax, 1

		ret

error:          mov	ebx, 80000000h
		xor	eax, eax

		mov	[Xp], ebx
		mov	[Yp], ebx

		fsubp	st(0), st(0)
		fsubp	st(0), st(0)
		fsubp	st(0), st(0)

		ret


LongWorldRotateProjectPoint3DF endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		END
