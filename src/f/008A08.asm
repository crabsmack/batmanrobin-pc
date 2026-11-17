; ---------------------------------------------------------------------------
; Obj_New - creates a one-block object on the main list: pops a pool block, sets its handler a0 and links it before g_ObjListTail; the rest of the block is left as it was
; In:  a0 = handler
; Out: d0 = 0 and a6 = the object (a0 = the previous last object, a1 = g_ObjListTail); or d0 = -1 (g_ObjPoolFree clamped to 0) when the pool is empty
; Evidence: callers VM4_Fork $60C4 and VM4_SpawnChild $613A (handler VM4_RunObject), Wave_Start $932E, Stage1 effects $20A48 / $20DDA; TAS 20277 frames
; ---------------------------------------------------------------------------
Obj_New:
	subq.b	#$1,(g_ObjPoolFree).w
	bcs.s	loc_008A32
	movea.w	(g_ObjPoolHead).w,a6
	move.w	(a6),(g_ObjPoolHead).w
	move.l	a0,(a6)
	lea.l	(g_ObjListTail).w,a1
	movea.w	$4(a1),a0
	move.w	a6,$4(a1)
	move.w	a6,$6(a0)
	movem.w	a0-a1,$4(a6)
	moveq	#$0,d0
	rts
loc_008A32:
	clr.b	(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
; ---------------------------------------------------------------------------
; Obj_UnlinkFreeWithBlocks - unlinks object a6 from its list (prev.next = next, next.prev = prev) and falls into Obj_FreeWithBlocks to free it and its handle blocks
; In:  a6 = linked object
; Out: a6 and its blocks back on the pool
; Evidence: jmp from Obj_Destroy when +4(a6) is nonzero and from Stage1_SignDebrisRun at the end of its life; movem.w 4(a6),a0-a1 / move.w a1,6(a0) / move.w a0,4(a1)
; ---------------------------------------------------------------------------
Obj_UnlinkFreeWithBlocks:
	movem.w	$4(a6),a0-a1
	move.w	a1,$6(a0)
	move.w	a0,$4(a1)
