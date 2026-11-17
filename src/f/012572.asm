; ---------------------------------------------------------------------------
; Layer_SetPlanes64x64 - scene setup callback: VDP planes are 64x64 cells (plane A $C000, plane B $E000, $80 bytes a row, $2000 a plane) and a screen redraw is 22x16 blocks
; In:  none
; Out: g_PlaneAVRAM, g_PlaneBVRAM, g_PlaneRowBytes, g_PlaneBytes, g_PlaneCellsX/Y = $40/$40, g_ScreenBlocksX/Y = $16/$10
; Evidence: pointed to from 12 scene setups ($01FBD2, $03FF32, $047548, ...); runs at each scene start (TAS 14-128753, idle, options); the values are read by Layer_Init ($36-$40 of the layer) and the scene-script ops $7186/$7256/$74A8
; ---------------------------------------------------------------------------
Layer_SetPlanes64x64:
	move.w	#$C000,(g_PlaneAVRAM).w
	move.w	#$E000,(g_PlaneBVRAM).w
	move.w	#$80,(g_PlaneRowBytes).w
	move.w	#$2000,(g_PlaneBytes).w
	move.w	#$40,(g_PlaneCellsX).w
	move.w	#$40,(g_PlaneCellsY).w
	move.w	#$16,(g_ScreenBlocksX).w
	move.w	#$10,(g_ScreenBlocksY).w
	rts
; ---------------------------------------------------------------------------
; Layer_SetPlanes128x32 - scene setup callback: VDP planes are 128x32 cells ($100 bytes a row, $2000 a plane, A $C000, B $E000), screen redraw 22x16 blocks
; In:  none
; Out: as Layer_SetPlanes64x64 with g_PlaneRowBytes $100, g_PlaneCellsX/Y = $80/$20
; Evidence: the other preset beside Layer_SetPlanes64x64; pointed to from 12 scene setups ($01FA7E, $032E16, $0476F2, ...); executed in 22 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Layer_SetPlanes128x32:
	move.w	#$C000,(g_PlaneAVRAM).w
	move.w	#$E000,(g_PlaneBVRAM).w
	move.w	#$100,(g_PlaneRowBytes).w
	move.w	#$2000,(g_PlaneBytes).w
	move.w	#$80,(g_PlaneCellsX).w
	move.w	#$20,(g_PlaneCellsY).w
	move.w	#$16,(g_ScreenBlocksX).w
	move.w	#$10,(g_ScreenBlocksY).w
	rts
