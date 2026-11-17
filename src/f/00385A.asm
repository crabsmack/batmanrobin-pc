; ---------------------------------------------------------------------------
; Obj_SetBodyBoxes - rebuilds the script object's body-box set (the boxes it can be struck through) from d7 inline specs, as Obj_SetAttackBoxes does, also clearing each box's damage taken (+$20); surplus boxes are unlinked from their list (when +2 is set) and freed
; In:  d7 = box count; a2 = specs (words kind, left, top, right, bottom, word n, callback + args); a4/a6 = object
; Out: a4 +$2 = count, +$4.. = handles, (a4) = 0; a2 past the specs
; Evidence: lea $4(a4),a1 / clr.w $20(a0) / Hitbox_Unlink / ObjPool_Free; the op $4EDA places these boxes each frame (rect from +6..+$E around the object, owner energy $52 -> +$1E, links by kind) and Obj_TestStruck reads their struck bit; 153 runs
; ---------------------------------------------------------------------------
Obj_SetBodyBoxes:
	clr.b	(a4)
	lea.l	$4(a4),a1
	moveq	#$0,d6
	bra.s	loc_0038B6
loc_003864:
	move.w	(a1)+,d0
	movea.w	d0,a0
	bne.s	loc_003878
	bsr.w	ObjPool_Alloc
	bne.s	loc_0038E0
	move.w	a0,-$2(a1)
	clr.w	$2(a0)
loc_003878:
	move.l	#$82C,$30(a0)
	clr.w	$3C(a0)
	clr.w	$20(a0)
	move.w	a6,$2E(a0)
	move.w	$58(a6),$10(a0)
	move.w	$56(a6),d0
	movem.w	(a2)+,d1-d5
	movem.w	d0-d5,$4(a0)
	move.w	(a2)+,d0
	beq.s	loc_0038B4
	lea.l	$30(a0),a0
	move.l	(a2)+,(a0)+
	subq.w	#$3,d0
	blt.s	loc_0038B4
loc_0038AE:
	move.w	(a2)+,(a0)+
	dbra	d0,loc_0038AE
loc_0038B4:
	addq.w	#$1,d6
loc_0038B6:
	dbra	d7,loc_003864
loc_0038BA:
	move.w	$2(a4),d7
	move.w	d6,$2(a4)
	sub.w	d6,d7
	subq.w	#$1,d7
	bmi.s	loc_0038DE
loc_0038C8:
	movea.w	(a1),a0
	clr.w	(a1)+
	tst.w	$2(a0)
	beq.s	loc_0038D6
	bsr.w	Hitbox_Unlink
loc_0038D6:
	bsr.w	ObjPool_Free
	dbra	d7,loc_0038C8
loc_0038DE:
	rts
loc_0038E0:
	move.w	$A(a2),d0
	add.w	d0,d0
	lea.l	$C(a2,d0.w),a2
	dbra	d7,loc_0038E0
	bra.s	loc_0038BA
; ---------------------------------------------------------------------------
; Op2_BranchStruck - a2 op: branches when one of the object's body boxes was struck (Obj_TestStruck; the box goes to g_StruckBox)
; In:  a2 = script: word offset
; Out: branch via Op2_Goto when struck, else NEXT after the offset
; Evidence: bsr Obj_TestStruck / bne Op2_Goto; 137 runs
; ---------------------------------------------------------------------------
Op2_BranchStruck:
	bsr.w	Obj_TestStruck
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchNotStruck - a2 op: branches when none of the object's body boxes was struck since the last test
; In:  a2 = script: word offset
; Out: branch when not struck, else (struck: g_StruckBox set) NEXT after the offset
; Evidence: bsr Obj_TestStruck / beq Op2_Goto; 145 runs
; ---------------------------------------------------------------------------
Op2_BranchNotStruck:
	bsr.w	Obj_TestStruck
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
