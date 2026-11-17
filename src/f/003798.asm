; ---------------------------------------------------------------------------
; Obj_SetAttackBoxes - rebuilds the script object's attack-box set from d7 inline specs: reuses or allocates a pool block per box, stamps owner a6 (+$2E), owner id $58 (+$10), serial $56 (+4), kind and rectangle (+6..+$E), hit callback (+$30, default the rts $82C) and its argument words; frees the boxes beyond the new count
; In:  d7 = box count; a2 = specs: words kind, left, top, right, bottom, word n (0 = no callback, else n words: long callback + n-2 argument words); a4/a6 = object
; Out: a4 +$A = count, +$C.. = handles, (a4)+1 = 0; a2 past the specs (skipped on a failed ObjPool_Alloc)
; Evidence: lea $C(a4),a1 / ObjPool_Alloc / movem.w d0-d5,$4(a0) / move.l (a2)+,$30 ... / ObjPool_Free loop; the per-frame op $4DA4 tests these boxes ($A/$C(a4)) unless the delay byte $1(a4) runs; 153 runs
; ---------------------------------------------------------------------------
Obj_SetAttackBoxes:
	clr.b	$1(a4)
	lea.l	$C(a4),a1
	moveq	#$0,d6
	bra.s	loc_0037EE
loc_0037A4:
	move.w	(a1)+,d0
	movea.w	d0,a0
	bne.s	loc_0037B4
	bsr.w	ObjPool_Alloc
	bne.s	loc_00380E
	move.w	a0,-$2(a1)
loc_0037B4:
	move.l	#$82C,$30(a0)
	clr.w	$3C(a0)
	move.w	a6,$2E(a0)
	move.w	$58(a6),$10(a0)
	move.w	$56(a6),d0
	movem.w	(a2)+,d1-d5
	movem.w	d0-d5,$4(a0)
	move.w	(a2)+,d0
	beq.s	loc_0037EC
	lea.l	$30(a0),a0
	move.l	(a2)+,(a0)+
	subq.w	#$3,d0
	blt.s	loc_0037EC
loc_0037E6:
	move.w	(a2)+,(a0)+
	dbra	d0,loc_0037E6
loc_0037EC:
	addq.w	#$1,d6
loc_0037EE:
	dbra	d7,loc_0037A4
loc_0037F2:
	move.w	$A(a4),d7
	move.w	d6,$A(a4)
	sub.w	d6,d7
	subq.w	#$1,d7
	bmi.s	loc_00380C
loc_003800:
	movea.w	(a1),a0
	bsr.w	ObjPool_Free
	clr.w	(a1)+
	dbra	d7,loc_003800
loc_00380C:
	rts
loc_00380E:
	move.w	$A(a2),d0
	add.w	d0,d0
	lea.l	$C(a2,d0.w),a2
	dbra	d7,loc_00380E
	bra.s	loc_0037F2
; ---------------------------------------------------------------------------
; Op2_SetBodyBoxes - a2 op: replaces the object's body (vulnerable) boxes (a4 +$2 count, +$4 handles) with n boxes described inline (Obj_SetBodyBoxes)
; In:  a2 = script: word n, then n box specs (as Obj_SetAttackBoxes)
; Out: NEXT after the specs
; Evidence: move.w (a2)+,d7 / bsr Obj_SetBodyBoxes; 3 runs; $180EA `381E 0002 0008 FFE2 FFCB 001E FFFD 0000 ...`
; ---------------------------------------------------------------------------
Op2_SetBodyBoxes:
	move.w	(a2)+,d7
	bsr.w	Obj_SetBodyBoxes
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetBodyBox - a2 op: replaces the object's body boxes with one box described inline
; In:  a2 = script: one box spec
; Out: NEXT after the spec
; Evidence: moveq #1,d7 / bsr Obj_SetBodyBoxes; 151 runs; $198D2 `3828 0008 FFDC FFBD 0023 003A 0000` (kind 8 = enemy list, no callback)
; ---------------------------------------------------------------------------
Op2_SetBodyBox:
	moveq	#$1,d7
	bsr.w	Obj_SetBodyBoxes
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearBodyBoxes - a2 op: removes all of the object's body boxes (unlinks them from their list and frees them): the object can no longer be hit
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: moveq #0,d7 / bsr Obj_SetBodyBoxes; 139 runs
; ---------------------------------------------------------------------------
Op2_ClearBodyBoxes:
	moveq	#$0,d7
	bsr.w	Obj_SetBodyBoxes
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetBodyBoxId - a2 op: sets the id (+$10, normally the owner id $58) of every body box of the object
; In:  a2 = script: word id
; Out: NEXT
; Evidence: loop over $2(a4) handles from $4(a4): move.w d0,$10(a0); Hitbox_FindPlayerBoxOfId matches on +$10; 68 runs; $4B7F6 `383C 0005`
; ---------------------------------------------------------------------------
Op2_SetBodyBoxId:
	move.w	(a2)+,d0
	move.w	$2(a4),d7
	subq.w	#$1,d7
	bmi.w	VM2_Next
	lea.l	$4(a4),a1
loc_00384C:
	movea.w	(a1)+,a0
	move.w	d0,$10(a0)
	dbra	d7,loc_00384C
	movea.w	(a2)+,a0
	jmp	(a0)
