; ---------------------------------------------------------------------------
; Camera_IsLocked - native callback: d0 = 1 when g_CameraLockX holds a lock point, else 0 (for the switch op $2D40)
; In:  none
; Out: d0 = 0 or 1
; Evidence: script uses after op $2D40 at $1E94E, $1EE8C; 119 TAS frames
; ---------------------------------------------------------------------------
Camera_IsLocked:
	tst.l	(g_CameraLockX).w
	beq.s	loc_0093AA
	moveq	#$1,d0
	rts
loc_0093AA:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Camera_LockHere - native callback: locks the camera where it is (lock point = the screen centre's x)
; In:  none
; Out: d0 = 0
; Evidence: g_CameraLockX = (g_CameraX + $A0) << 16; Camera_TrackPlayersX then aims at it instead of the players; callers the stage loops $32F14, $33274, $3FFBE, $401DA, $479EE, $47BD0
; ---------------------------------------------------------------------------
Camera_LockHere:
	moveq	#$0,d0
	move.w	(g_CameraX).w,d0
	addi.w	#$A0,d0
	swap	d0
	move.l	d0,(g_CameraLockX).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Camera_LockRightEdge - level trigger callback: locks the camera so it scrolls on until the trigger's x d4 is at the screen's right edge
; In:  d4 = trigger x
; Out: d0 = 0
; Evidence: g_CameraLockX = (d4 - $A0) << 16 (camera aims its centre $A0 left of it); uses in the stage trigger tables ($16138...)
; ---------------------------------------------------------------------------
Camera_LockRightEdge:
	subi.w	#$A0,d4
	swap	d4
	clr.w	d4
	move.l	d4,(g_CameraLockX).w
	moveq	#$0,d0
	rts
