; ---------------------------------------------------------------------------
; Obj_NewMultiAfter - Obj_NewMulti linked right after the running object a6 instead of before the main list's tail: pops d7 pool blocks, makes the first the object (handler a0) and shares Obj_NewMulti's handle and clear runs
; In:  a0 = handler, d7 = blocks, a6 = the object to link after
; Out: d0 = 0 and a6 = the new object, a0 = the old next object, a1/a2/d1/d7 as Obj_NewMulti_FillHandles; or d0 = -1 through Obj_NewMultiFail
; Evidence: sub.b d7,(g_ObjPoolFree).w / bcs Obj_NewMultiFail / movea.w a6,a1 / the pool pop / the link between a1 and its next (6(a1)) / bra.w Obj_NewMulti_FillHandles; bsr target of Obj_SpawnAfter $008F40; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Obj_NewMultiAfter:
	sub.b	d7,(g_ObjPoolFree).w
	bcs.w	Obj_NewMultiFail
	movea.w	a6,a1
	movea.w	(g_ObjPoolHead).w,a6
	movea.w	(a6),a2
	move.l	a0,(a6)
	movea.w	$6(a1),a0
	move.w	a6,$6(a1)
	move.w	a6,$4(a0)
	move.w	a1,$4(a6)
	move.w	a0,$6(a6)
	lea.l	$8(a6),a1
	moveq	#$0,d0
	bra.w	Obj_NewMulti_FillHandles
