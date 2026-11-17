; ---------------------------------------------------------------------------
; Shake_Update - steps the screen-shake bounce and the second oscillator, and sets the view position (camera + shake)
; In:  g_ShakeY/Vel/Gravity, g_Shake2Pos/Vel/Pull
; Out: g_ViewY = g_CameraY + g_ShakeY, g_ViewX = g_CameraX
; Evidence: g_ShakeY += speed; going below 0 reverses the speed at 3/4 and rests it at 0; the oscillator stops once its speed is under 0.5 at a crossing; only caller Camera_UpdateView; 124729 TAS frames
; ---------------------------------------------------------------------------
Shake_Update:
	move.l	(g_ShakeY).w,d0
	move.l	(g_ShakeVel).w,d1
	sub.l	(g_ShakeGravity).w,d1
	add.l	d1,d0
	bpl.s	loc_00965C
	neg.l	d1
	move.l	d1,d2
	asr.l	#$2,d2
	sub.l	d2,d1
	moveq	#$0,d0
loc_00965C:
	move.l	d0,(g_ShakeY).w
	move.l	d1,(g_ShakeVel).w
	add.l	(g_CameraY).w,d0
	move.l	d0,(g_ViewY).w
	move.l	(g_CameraX).w,(g_ViewX).w
	move.l	(g_Shake2Vel).w,d0
	move.l	(g_Shake2Pos).w,d1
	bpl.s	loc_009686
	add.l	(g_Shake2Pull).w,d0
	add.l	d0,d1
	bmi.s	loc_0096AC
	bra.s	loc_00968E
loc_009686:
	sub.l	(g_Shake2Pull).w,d0
	add.l	d0,d1
	bpl.s	loc_0096AC
loc_00968E:
	move.l	d0,d2
	asr.l	#$2,d2
	sub.l	d2,d0
	bpl.s	loc_0096A0
	cmpi.l	#$FFFF8000,d0
	blt.s	loc_0096AC
	bra.s	loc_0096A8
loc_0096A0:
	cmpi.l	#$8000,d0
	bgt.s	loc_0096AC
loc_0096A8:
	moveq	#$0,d0
	moveq	#$0,d1
loc_0096AC:
	move.l	d0,(g_Shake2Vel).w
	move.l	d1,(g_Shake2Pos).w
	rts
