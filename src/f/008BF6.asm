; ---------------------------------------------------------------------------
; VIntObj_UnlinkFree - unlinks object a6 from the V-int list and returns its block to the object pool, with g_VIntOff raised so the V-int can't walk the list mid-edit
; In:  a6 = V-int list object
; Out: g_ObjPoolHead = a6, g_ObjPoolFree +1
; Evidence: callers VM6_EndClearJob and the blit job's end hook in $74A8 (free g_A6VIntJob after the last row); TAS 6 frames (f18897-f77202)
; ---------------------------------------------------------------------------
VIntObj_UnlinkFree:
	addq.b	#$1,(g_VIntOff).w
	movem.w	$4(a6),a0-a1
	move.w	a1,$6(a0)
	move.w	a0,$4(a1)
	move.w	(g_ObjPoolHead).w,(a6)
	move.w	a6,(g_ObjPoolHead).w
	addq.b	#$1,(g_ObjPoolFree).w
	subq.b	#$1,(g_VIntOff).w
	rts
