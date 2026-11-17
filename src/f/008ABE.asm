; ---------------------------------------------------------------------------
; Obj_UnlinkFree - unlinks the one-block object a6 from its list and pushes the block back on the object pool
; In:  a6 = object
; Out: g_ObjPoolHead = a6, g_ObjPoolFree +1; a0/a1 = its old neighbours
; Evidence: callers Op4_Kill tail $53D4 (bra), Op4_ShotStep (bcc), VM5_EndWave; the inverse of Obj_New; TAS 24669 frames
; ---------------------------------------------------------------------------
Obj_UnlinkFree:
	movem.w	$4(a6),a0-a1
	move.w	a1,$6(a0)
	move.w	a0,$4(a1)
	move.w	(g_ObjPoolHead).w,(a6)
	move.w	a6,(g_ObjPoolHead).w
	addq.b	#$1,(g_ObjPoolFree).w
	rts
