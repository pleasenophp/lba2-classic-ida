;*══════════════════════════════════════════════════════════════════════════*

		.386p

		.model SMALL, C

		include mat.inc

;*══════════════════════════════════════════════════════════════════════════*

		.DATA

		extrn	C CameraX:DWORD
		extrn	C CameraY:DWORD
		extrn	C CameraZ:DWORD

		extrn	C XCentre:DWORD
		extrn	C YCentre:DWORD

		extrn	C	Xp:DWORD
		extrn	C	Yp:DWORD

		ASSUME  DS:SEG CameraX

;*══════════════════════════════════════════════════════════════════════════*

		.CODE

		public C	LongRotateProjectPointIsoI

;*══════════════════════════════════════════════════════════════════════════*
;void	LongRotateProjectPointIsoI(S32 *Mat, S32 x, S32 y, S32 z)	;

;#pragma aux LongRotateProjectPointIsoI		\
;	parm caller	[esi] [eax] [ebx] [ecx]	\
;	modify exact	[eax ebx ecx edx edi]

LongRotateProjectPointIsoI proc

		push	ebp
		mov	[Yp], eax

		imul	[esi].Struc_MatriceMAT.MAT_M11

		mov     ebp, eax
 		mov	eax, [esi].Struc_MatriceMAT.MAT_M12
		mov     edi, edx

		imul	ebx

		add     ebp, eax
		mov	eax, [esi].Struc_MatriceMAT.MAT_M13
		adc     edi, edx

		imul	ecx

		add     ebp, eax
		mov     eax, [esi].Struc_MatriceMAT.MAT_M21
		adc     edi, edx

		imul    [Yp]
		shrd    ebp, edi, 14

		adc	ebp, 0
		mov     edi, edx
		mov	[Xp], ebp
		mov     ebp, eax
		mov	eax, [esi].Struc_MatriceMAT.MAT_M22

		imul	ebx

		add     ebp, eax
		mov	eax, [esi].Struc_MatriceMAT.MAT_M23
		adc     edi, edx

		imul	ecx

		add     ebp, eax
		mov     eax, [esi].Struc_MatriceMAT.MAT_M31
		adc     edi, edx

		imul    [Yp]
		shrd    ebp, edi, 14

		adc	ebp, 0
		mov     edi, edx
		mov	[Yp], ebp
		mov     ebp, eax
		mov	eax, [esi].Struc_MatriceMAT.MAT_M32

		imul	ebx

		add     ebp, eax
		mov	eax, [esi].Struc_MatriceMAT.MAT_M33
		adc     edi, edx

		imul	ecx

		add     eax, ebp
		pop	ebp
		adc     edx, edi

		shrd    eax, edx, 14

		adc	eax, 0
		mov	ecx, [Xp]

		mov	ebx, [Yp]
		sub	ecx, CameraX

		sub	ebx, CameraY
		sub	eax, CameraZ

		mov	edx, ecx		; save x
		sub	ecx, eax		; x + zrot

		add	eax, edx		; -(x - zrot)
		mov	edx, ebx		; save y

		shl	ebx, 4			; *16
		add	eax, eax		; *2

		lea	ecx, [ecx+ecx*2]	; *3
		sub	ebx, edx		; y*15

		sar	ecx, 6			; /64 IsoScale
		lea	eax, [eax+eax*2]	; *3

		add	ecx, [XCentre]
		sub	eax, ebx		; - (x-zrot) * 6

		sar	eax, 8			; /256 IsoScale
		mov	[Xp], ecx

		add	eax, [YCentre]

		inc	eax			; old lib

		mov	[Yp], eax
		mov	eax, -1

		ret

LongRotateProjectPointIsoI	endp

;*══════════════════════════════════════════════════════════════════════════*
;		The
		END
