;****************************************************************************

		.386p

		.model SMALL, C

		include 3d.inc

;****************************************************************************
		.DATA

;****************************************************************************
		public	C AlphaLight
		public	C BetaLight
AlphaLight	dd	0000
BetaLight	dd	0000

		public	C CameraXLight
		public	C CameraYLight
		public	C CameraZLight
CameraXLight	dd	LIB_NORMAL_UNIT
CameraYLight	dd	0000
CameraZLight	dd	0000

		public	C NormalXLight
		public	C NormalYLight
		public	C NormalZLight
NormalXLight	dd	LIB_NORMAL_UNIT
NormalYLight	dd	0000
NormalZLight	dd	0000

		public	C FactorLight
FactorLight	REAL4	1.52587890625E-5	;	1.0 / 65536.0

		public	C PosXLight
		public	C PosYLight
		public	C PosZLight
PosXLight	dd	0000
PosYLight	dd	0000
PosZLight	dd	0000

		public	C TargetXLight
		public	C TargetYLight
		public	C TargetZLight
TargetXLight	dd	0000
TargetYLight	dd	0000
TargetZLight	dd	0000

		public	C LightNormalUnit
LightNormalUnit	dd	LIB_NORMAL_UNIT

ASSUME ds:SEG LightNormalUnit

;****************************************************************************
		extrn	C	MatriceWorld:DWORD
		extrn	C	MatrixLib2:DWORD

		extrn	C	X0:DWORD
		extrn	C	Y0:DWORD
		extrn	C	Z0:DWORD

		extrn	C	InitMatrixStd:DWORD
		extrn	C	RotatePoint:DWORD
		extrn	C	InverseRotatePoint:DWORD

;****************************************************************************
		.CODE

;****************************************************************************
		extrn	C	GetAngleVector3D:PROC

;****************************************************************************
		public	C SetPosLight
		public	C SetTargetLight
		public	C SetLightVector

;****************************************************************************
;void	SetPosLight(S32 x, S32 y, S32 z);

;#pragma aux SetPosLight	"*"		\
;	parm		[eax] [ebx] [ecx]	\
;	modify		[edx esi edi]

SetPosLight	proc

		mov	[PosXLight], eax
		mov	[PosYLight], ebx
		mov	[PosZLight], ecx

                mov     eax, [TargetXLight]
                mov     ebx, [TargetYLight]
                mov     ecx, [TargetZLight]

		jmp	ComputeLightVector

SetPosLight	endp

;****************************************************************************
;void	RecomputeLight();

;#pragma aux RecomputeLight	"*"		\
;	modify		[eax ebx ecx edx esi edi]

RecomputeLight	proc

		mov	eax, [NormalXLight]
		mov	ebx, [NormalYLight]

		mov	ecx, [NormalZLight]

		mov	esi, offset MatriceWorld
		call	[RotatePoint]

		mov	eax, [X0]
		mov	ebx, [Y0]

		mov	ecx, [Z0]
		mov	[CameraXLight], eax

		mov	[CameraYLight], ebx
		mov	[CameraZLight], ecx

		ret


RecomputeLight	endp

;****************************************************************************
;void	SetTargetLight(S32 targetx, S32 targety, S32 targetz);

;#pragma aux SetTargetLight	"*"		\
;	parm		[eax] [ebx] [ecx]	\
;	modify		[edx esi edi]

SetTargetLight	proc

                mov     [TargetXLight], eax
                mov     [TargetYLight], ebx

                mov     [TargetZLight], ecx
ComputeLightVector::
		sub	eax, [PosXLight]

		sub	ebx, [PosYLight]
		sub	ecx, [PosZLight]

		call	GetAngleVector3D

		mov	eax, [X0]
		mov	ebx, [Y0]

		push ebx
		push eax
		call SetLightVector
		add esp, 4*2
		ret

SetTargetLight endp

;****************************************************************************
;void	SetLightVector( S32 alpha, S32 beta) ;

;#pragma aux SetLightVector	"*"		\
;	parm		[eax] [ebx]		\
;	modify		[ecx edx esi edi]

SetLightVector PROC \
			uses eax ebx ecx edx edi esi ebp\
			alpha: DWORD, beta: DWORD
			mov eax, alpha
			mov ebx, beta


		and	eax, 4095
		and	ebx, 4095

		xor	ecx, ecx
		mov	[AlphaLight], eax

		mov	[BetaLight], ebx
		mov	edi, offset MatrixLib2

		call	[InitMatrixStd]

		xor	eax, eax
		xor	ebx, ebx

		mov	ecx, [LightNormalUnit]
		mov	esi, offset MatrixLib2

		call	[InverseRotatePoint]

		mov	eax, [X0]
		mov	ebx, [Y0]

		mov	ecx, [Z0]
		mov	[NormalXLight], eax

		mov	[NormalYLight], ebx
		mov	[NormalZLight], ecx
		mov	esi, offset MatriceWorld
		call	[RotatePoint]

		mov	eax, [X0]
		mov	ebx, [Y0]

		mov	ecx, [Z0]
		mov	[CameraXLight], eax

		mov	[CameraYLight], ebx
		mov	[CameraZLight], ecx

		ret

SetLightVector	endp

;****************************************************************************
;		The
		End
