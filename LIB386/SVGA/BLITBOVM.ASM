;----------------------------------------------------------------------------
			.386

			.MODEL	FLAT, C

			include	iammx.inc

;----------------------------------------------------------------------------
			SEG_FS	equ	db 64h
;----------------------------------------------------------------------------
			.DATA

;----------------------------------------------------------------------------
			extrn	C TabOffPhysLine:DWORD

;----------------------------------------------------------------------------
			.CODE

;----------------------------------------------------------------------------
			extrn	C	VirtualLinearFirst:PROC

;----------------------------------------------------------------------------
			public	C	BlitBoxVESAM
			public	C	BlitBoxVESAM1
			public	C	BlitBoxVESAM2

;----------------------------------------------------------------------------
;void	BlitBoxVESAM(void *dst, void *src) ;

;#pragma aux BlitBoxVESAM	\
;	parm	[edi] [esi]	\
;	modify	[eax ebx ecx edx]

BlitBoxVESAM		PROC
			ASSUME	FS:_DATA

			mov	edx, offset TabOffPhysLine
			mov	ecx, 200

			mov	ebx, [edx+140*4]
			mov	eax, -320

			mov	edx, [edx+1*4]
			lea	edi, [edi+ebx+160+320]

			call	VirtualLinearFirst
loopCopy:
			movq	mm0, [esi]
			movq	mm1, [esi+8]
			add	esi, 16
BlitBoxVESAM1::		SEG_FS
			movq	[edi+eax], mm0
BlitBoxVESAM2::		SEG_FS
			movq	[edi+eax+8], mm1
			add	eax, 16
			jnz	loopCopy
			mov	eax, -320
			add	edi, edx
			dec	ecx
			jnz	loopCopy

			ret

BlitBoxVESAM		ENDP

;----------------------------------------------------------------------------
;			The
			End
