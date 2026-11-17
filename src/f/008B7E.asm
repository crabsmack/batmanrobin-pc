; ---------------------------------------------------------------------------
; VIntObj_New - creates a one-block object on the V-int list: pops a pool block, sets its handler a0 and links it before g_VIntListTail, with g_VIntOff raised so a V-int during the link is skipped
; In:  a0 = handler
; Out: d0 = 0 and a6 = the object; or d0 = -1 (g_ObjPoolFree clamped to 0) when the pool is empty
; Evidence: callers the stage setups $1FB3A/$1FC50/$1FD76/$1FECC and others (Layer_FlushTileQueueIfIdle, Stage1_VIntFacade); TAS 11 frames; raw $8B3E (append an existing a0) and $8B62 (unlink a0) after VIntObj_InitList never run
; ---------------------------------------------------------------------------
VIntObj_New:
	subq.b	#$1,(g_ObjPoolFree).w
	bcs.s	loc_008BB0
	addq.b	#$1,(g_VIntOff).w
	movea.w	(g_ObjPoolHead).w,a6
	move.w	(a6),(g_ObjPoolHead).w
	move.l	a0,(a6)
	lea.l	(g_VIntListTail).w,a1
	movea.w	$4(a1),a0
	move.w	a6,$4(a1)
	move.w	a6,$6(a0)
	movem.w	a0-a1,$4(a6)
	subq.b	#$1,(g_VIntOff).w
	moveq	#$0,d0
	rts
loc_008BB0:
	clr.b	(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
