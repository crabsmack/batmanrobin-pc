; ---------------------------------------------------------------------------
; Obj_Destroy - tears down the current script object: notifies its spawn group (SpawnAction_GroupMemberGone with $1A(a6)) when its spawn action $19(a6) is 1, subtracts its population weight $18(a6) from g_ObjPopulation, releases its sprite frames and collision boxes, then unlinks it from its object list (when +4 prev is set) and returns its pool blocks
; In:  a6 = object, a3/a4/a5 = its blocks
; Out: the object and its blocks are free (jmp Obj_UnlinkFreeWithBlocks / Obj_FreeWithBlocks); registers clobbered
; Evidence: callers Op2_Kill (the off-screen kill ops $8A2-$93A) and Obj_DestroyOther ($966, used by op $24CE on a child handle); calls Obj_ReleaseFrames, Obj_SetBodyBoxes and Obj_SetAttackBoxes with d7 = 0 (each frees its list of pool blocks at $4(a4)/$C(a4), inferred: 'set 0 boxes'); 2.2k TAS frames
; ---------------------------------------------------------------------------
Obj_Destroy:
	cmpi.b	#$1,$19(a6)
	bne.w	loc_000992
	move.w	$1A(a6),d1
	bsr.w	SpawnAction_GroupMemberGone
loc_000992:
	moveq	#$0,d0
	move.b	$18(a6),d0
	sub.w	d0,(g_ObjPopulation).w
	bcc.s	loc_0009A2
	clr.w	(g_ObjPopulation).w
loc_0009A2:
	bsr.w	Obj_ReleaseFrames
	moveq	#$0,d7
	bsr.w	Obj_SetBodyBoxes
	moveq	#$0,d7
	bsr.w	Obj_SetAttackBoxes
	tst.w	$4(a6)
	beq.s	loc_0009BE
	jmp	(Obj_UnlinkFreeWithBlocks).l
loc_0009BE:
	jmp	(Obj_FreeWithBlocks).l
