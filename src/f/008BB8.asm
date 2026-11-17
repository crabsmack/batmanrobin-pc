; ---------------------------------------------------------------------------
; VIntObj_NewIdle - VIntObj_New with the handler fixed to its own closing rts ($8BEC), so the object does nothing until the caller stores the real handler in (a6)
; In:  none
; Out: d0 = 0 and a6 = the object; or d0 = -1 when the pool is empty
; Evidence: callers VM6_StartClearJob $72D2 (then move.l #$7302,(a6)), the map blit setup $74A8 (#$759E), $1C3F2 (its own handler); move.l #$8BEC,(a6) = the rts at the end of this routine; TAS 7 frames (f18887-f77184)
; ---------------------------------------------------------------------------
VIntObj_NewIdle:
	subq.b	#$1,(g_ObjPoolFree).w
	bcs.s	loc_008BEE
	addq.b	#$1,(g_VIntOff).w
	movea.w	(g_ObjPoolHead).w,a6
	move.w	(a6),(g_ObjPoolHead).w
	move.l	#VIntObj_IdleHandler,(a6)
	lea.l	(g_VIntListTail).w,a1
	movea.w	$4(a1),a0
	move.w	a6,$4(a1)
	move.w	a6,$6(a0)
	movem.w	a0-a1,$4(a6)
	subq.b	#$1,(g_VIntOff).w
	moveq	#$0,d0
; ---------------------------------------------------------------------------
; VIntObj_IdleHandler - the do-nothing V-int list handler VIntObj_NewIdle gives a new object: the rts that ends VIntObj_NewIdle's success path
; In:  a6 = the object (unused)
; Out: none
; Evidence: move.l #VIntObj_IdleHandler,(a6) in VIntObj_NewIdle (the asm had #$8BEC); VIntObj_RunList's jsr (a5) reaches it if a V-int runs before the caller stores the real handler
; ---------------------------------------------------------------------------
VIntObj_IdleHandler:
	rts
loc_008BEE:
	clr.b	(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
