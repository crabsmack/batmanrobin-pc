; ---------------------------------------------------------------------------
; Obj_NewMulti_Clear4 - the clear-run entry used by 4-block objects (5 handle words left to zero), then the end of Obj_NewMulti: stores a2 as the new pool head and returns success
; In:  a1 = object +$E, a2 = first block not taken, d0 = 0
; Out: g_ObjPoolHead = a2, d0 = 0
; Evidence: jmp $896A(pc,d1.w) with d1 = 8 lands here for Obj_Spawn's 4-block script objects: ctarget $8972 in 140 corpus runs; the 3 move.w before it are the 1-3 block entries
; ---------------------------------------------------------------------------
Obj_NewMulti_Clear4:
	move.w	d0,(a1)+
; ---------------------------------------------------------------------------
; Obj_NewMulti_Clear5 - the clear-run entry used by 5-block objects (4 handle words left to zero), then the end of Obj_NewMulti: stores a2 as the new pool head and returns success
; In:  a1 = object +$10, a2 = first block not taken, d0 = 0
; Out: g_ObjPoolHead = a2, d0 = 0
; Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 10 lands at $8974: a runtime entry of the reference (call-site log); one move.w of Obj_NewMulti_Clear4's run
; ---------------------------------------------------------------------------
Obj_NewMulti_Clear5:
	move.w	d0,(a1)+
; ---------------------------------------------------------------------------
; Obj_NewMulti_Clear6 - the clear-run entry used by 6-block objects (3 handle words left to zero), then the end of Obj_NewMulti: stores a2 as the new pool head and returns success
; In:  a1 = object +$12, a2 = first block not taken, d0 = 0
; Out: g_ObjPoolHead = a2, d0 = 0
; Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 12 lands at $8976: a runtime entry of the reference (call-site log); one move.w of Obj_NewMulti_Clear4's run
; ---------------------------------------------------------------------------
Obj_NewMulti_Clear6:
	move.w	d0,(a1)+
; ---------------------------------------------------------------------------
; Obj_NewMulti_Clear7 - the clear-run entry used by 7-block objects (2 handle words left to zero), then the end of Obj_NewMulti: stores a2 as the new pool head and returns success
; In:  a1 = object +$14, a2 = first block not taken, d0 = 0
; Out: g_ObjPoolHead = a2, d0 = 0
; Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 14 lands at $8978: a runtime entry of the reference (call-site log); one move.w of Obj_NewMulti_Clear4's run
; ---------------------------------------------------------------------------
Obj_NewMulti_Clear7:
	move.w	d0,(a1)+
; ---------------------------------------------------------------------------
; Obj_NewMulti_Clear8 - the clear-run entry used by 8-block objects (1 handle word left to zero), then the end of Obj_NewMulti: stores a2 as the new pool head and returns success
; In:  a1 = object +$16, a2 = first block not taken, d0 = 0
; Out: g_ObjPoolHead = a2, d0 = 0
; Evidence: jmp $896A(pc,d1.w) at $8968 with d1 = 16 lands at $897A: a runtime entry of the reference (call-site log); one move.w of Obj_NewMulti_Clear4's run
; ---------------------------------------------------------------------------
Obj_NewMulti_Clear8:
	move.w	d0,(a1)+
	move.w	a2,(g_ObjPoolHead).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Obj_NewMultiUnlinked - Obj_NewMulti without the list insert: pops d7 blocks, sets the handler a0 and zero prev/next links, then shares Obj_NewMulti's handle and clear runs (bra $893C)
; In:  a0 = handler, d7 = blocks
; Out: d0 = 0 and a6 = the object (not on any list), or d0 = -1 through Obj_NewMultiFail
; Evidence: caller Obj_SpawnUnlinked $8F24 (the ...Unlinked spawners; $29250 links such an object later with Obj_LinkAtEnd); move.l a0,(a1)+ / move.l d0,(a1)+ then bra $893C = Obj_NewMulti's `add.w d7,d7` computed jump
; ---------------------------------------------------------------------------
Obj_NewMultiUnlinked:
	sub.b	d7,(g_ObjPoolFree).w
	bcs.s	Obj_NewMultiFail
	movea.w	(g_ObjPoolHead).w,a6
	movea.w	(a6),a2
	movea.l	a6,a1
	moveq	#$0,d0
	move.l	a0,(a1)+
	move.l	d0,(a1)+
	bra.s	Obj_NewMulti_FillHandles
