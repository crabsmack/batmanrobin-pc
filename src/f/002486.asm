; ---------------------------------------------------------------------------
; Op2_SpawnChildOrWait - a2 op: as Op2_SpawnChild, but when the spawn fails (pool full) it rewinds to itself and yields, retrying next frame
; In:  a2 = script: word a6 field offset, long spawner
; Out: field = child a6; NEXT, or a2 back 8 bytes and Op2_Yield on failure
; Evidence: same body as $244A with d1 = 8, then suba.w d1,a2 / bra Op2_Yield; dispatched in 9 runs (6 TAS frames)
; ---------------------------------------------------------------------------
Op2_SpawnChildOrWait:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	moveq	#$8,d1
	move.w	(a2)+,d0
	movea.l	(a2)+,a0
	tst.w	(a6,d0.w)
	bne.w	VM2_Next
	movem.l	d0-d1/a2-a6,-(a7)
	jsr	(a0)
	movea.l	a6,a0
	movem.l	(a7)+,d0-d1/a2-a6
	bne.w	loc_0024B4
	move.w	a0,(a6,d0.w)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0024B4:
	suba.w	d1,a2
	bra.w	Op2_Yield
; ---------------------------------------------------------------------------
; Op2_CallNativeOnChild - a2 op: calls a native routine with a0 = the child object (a6 field operand); skips the call when there is no child
; In:  a2 = script: word a6 field offset, long routine
; Out: NEXT
; Evidence: move.w (a6,d0.w),d1 / beq NEXT / movea.w d1,a0 / jsr (a1); dispatched in 22 runs
; ---------------------------------------------------------------------------
Op2_CallNativeOnChild:
	move.w	(a2)+,d0
	movea.l	(a2)+,a1
	move.w	(a6,d0.w),d1
	beq.w	VM2_Next
	movea.w	d1,a0
	jsr	(a1)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KillChild - a2 op: destroys the child object in the a6 field operand (Obj_DestroyOther) and clears the field
; In:  a2 = script: word a6 field offset
; Out: NEXT
; Evidence: clr.w (a6,d0.w) / bsr Obj_DestroyOther; dispatched in 1 run (TAS)
; ---------------------------------------------------------------------------
Op2_KillChild:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d1
	beq.w	VM2_Next
	clr.w	(a6,d0.w)
	movea.w	d1,a0
	bsr.w	Obj_DestroyOther
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchHasChild - a2 op: branches while the a6 field operand holds a child object
; In:  a2 = script: word a6 field offset, word branch offset
; Out: branch via $2D22 when nonzero, else NEXT after the offset
; Evidence: tst.w (a6,d0.w) / bne $2D22; dispatched in 27 runs
; ---------------------------------------------------------------------------
Op2_BranchHasChild:
	move.w	(a2)+,d0
	tst.w	(a6,d0.w)
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchNoChild - a2 op: branches when the a6 field operand holds no child object (0)
; In:  a2 = script: word a6 field offset, word branch offset
; Out: branch via $2D22 when 0, else NEXT after the offset
; Evidence: tst.w (a6,d0.w) / beq $2D22; dispatched in 37 runs
; ---------------------------------------------------------------------------
Op2_BranchNoChild:
	move.w	(a2)+,d0
	tst.w	(a6,d0.w)
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SyncAndRunChild - a2 op: Op2_SyncChild (copy chosen fields to the child) and then, when there is a child, runs its object handler at once as Op2_RunChild does
; In:  a2 = script: word a6 field offset, word flags (+ offset pairs, see Obj_SyncChild)
; Out: NEXT; field cleared when the child's handler ends it
; Evidence: bsr Obj_SyncChild / tst.w d1 / bne $251C; dispatched in 24 runs
; ---------------------------------------------------------------------------
Op2_SyncAndRunChild:
	bsr.w	Obj_SyncChild
	tst.w	d1
	bne.s	loc_00251C
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_RunChild - a2 op: runs the child object's handler now (a6 = child, jsr (a6)) with g_VM2SavedSP saved around it; clears the field when the handler returns Z clear (the child ended)
; In:  a2 = script: word a6 field offset
; Out: NEXT
; Evidence: move.l g_VM2SavedSP,-(a7) / movea.w d1,a6 / movea.l (a6),a0 / jsr (a0) / bne -> clr.w (a6,d0.w); dispatched in 22 runs; nested script runs must keep g_VM2SavedSP (NOTES)
; ---------------------------------------------------------------------------
Op2_RunChild:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d1
	beq.w	VM2_Next
loc_00251C:
	move.l	(g_VM2SavedSP).w,-(a7)
	movem.l	d0/a1-a6,-(a7)
	movea.w	d1,a6
	movea.l	(a6),a0
	jsr	(a0)
	bne.s	loc_002538
	movem.l	(a7)+,d0/a1-a6
	move.l	(a7)+,(g_VM2SavedSP).w
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002538:
	movem.l	(a7)+,d0/a1-a6
	move.l	(a7)+,(g_VM2SavedSP).w
	clr.w	(a6,d0.w)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SyncChild - a2 op: copies the parent's position/display fields to its child as selected by a flags word (Obj_SyncChild)
; In:  a2 = script: word a6 field offset, word flags, optional offset pairs
; Out: NEXT
; Evidence: bsr Obj_SyncChild; dispatched in 13 runs
; ---------------------------------------------------------------------------
Op2_SyncChild:
	bsr.w	Obj_SyncChild
	movea.w	(a2)+,a0
	jmp	(a0)
