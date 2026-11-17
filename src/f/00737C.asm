; ---------------------------------------------------------------------------
; Op6_MoveLayerAToCamera - a6 op: places map layer g_LayerA at the camera position plus an offset (Layer_MoveX/Layer_MoveY, no drawing) and continues
; In:  a6 = script: word dx, word dy (pixels, added to g_CameraX/g_CameraY)
; Out: continues (a6 reloaded)
; Evidence: 2 script uses: $330BE and $331E0 `737C FEAC 002C` (camera - $154, + $2C), each followed by Op6_RedrawLayerA; the raw $7366-$7378 / $73A6-$73BC variants use 0 or only one camera axis, or the layer at $EDE8; TAS f67239
; ---------------------------------------------------------------------------
Op6_MoveLayerAToCamera:
	move.w	(g_CameraX).w,d0
	move.w	(g_CameraY).w,d1
	add.w	(a6)+,d0
	add.w	(a6)+,d1
	move.l	a6,(g_A6ScriptPc).w
	lea.l	(g_LayerA).l,a6
	move.w	d1,-(a7)
	jsr	(Layer_MoveX).l
	move.w	(a7)+,d0
	jsr	(Layer_MoveY).l
	bra.w	VM6_Reload
