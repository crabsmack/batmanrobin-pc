; ---------------------------------------------------------------------------
; Obj_NewClear - creates a one-block object on the main list: pops a pool block, sets its handler a0, links it before g_ObjListTail and zeroes +8..+$17
; In:  a0 = handler
; Out: d0 = 0 and a6 = the object; or d0 = -1 (g_ObjPoolFree clamped to 0) when the pool is empty
; Evidence: callers the stage player setup $99D4 (two player-slot objects with Player_JoinOrSpawn), Stage1_StartRubbleRain $20A48, $49798; TAS 18 frames; same as Obj_New plus 4 move.l of 0 from +8
; ---------------------------------------------------------------------------
Obj_NewClear:
	subq.b	#$1,(g_ObjPoolFree).w
	bcs.s	loc_008A00
	movea.w	(g_ObjPoolHead).w,a6
	move.w	(a6),(g_ObjPoolHead).w
	move.l	a0,(a6)
	lea.l	(g_ObjListTail).w,a1
	movea.w	$4(a1),a0
	move.w	a6,$4(a1)
	move.w	a6,$6(a0)
	movem.w	a0-a1,$4(a6)
	lea.l	$8(a6),a0
	moveq	#$0,d0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	rts
loc_008A00:
	clr.b	(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
