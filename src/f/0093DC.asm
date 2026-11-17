; ---------------------------------------------------------------------------
; Camera_UpdateView - per frame: steps the screen shake and derives the view values from the camera (sprite offsets, on-screen and movement areas), and counts down two timers
; In:  g_CameraX/Y
; Out: g_SprOffsetX/Y = $20 - g_ViewX/Y, g_OnScreenLeft/Top = camera - $18, g_MoveAreaLeft/Top = camera + g_MoveAreaOfsX/Y
; Evidence: g_ScreenClearTimer and g_ScriptTimer -1 down to 0; tail of Camera_TrackPlayersX and called by the other stage loops ($15916, $1FD76, $3FE94, $400E4...); 124699 TAS frames
; ---------------------------------------------------------------------------
Camera_UpdateView:
	bsr.w	Shake_Update
	move.w	#$20,d1
	sub.w	(g_ViewY).w,d1
	move.w	d1,(g_SprOffsetY).w
	move.w	#$20,d1
	sub.w	(g_ViewX).w,d1
	move.w	d1,(g_SprOffsetX).w
	move.w	(g_CameraX).w,d0
	subi.w	#$18,d0
	move.w	d0,(g_OnScreenLeft).w
	move.w	(g_CameraY).w,d0
	subi.w	#$18,d0
	move.w	d0,(g_OnScreenTop).w
	move.l	(g_CameraX).w,d0
	add.l	(g_MoveAreaOfsX).w,d0
	move.l	d0,(g_MoveAreaLeft).w
	move.l	(g_CameraY).w,d0
	add.l	(g_MoveAreaOfsY).w,d0
	move.l	d0,(g_MoveAreaTop).w
	tst.w	(g_ScreenClearTimer).w
	beq.s	loc_009432
	subq.w	#$1,(g_ScreenClearTimer).w
loc_009432:
	tst.w	(g_ScriptTimer).w
	beq.s	loc_00943C
	subq.w	#$1,(g_ScriptTimer).w
loc_00943C:
	rts
