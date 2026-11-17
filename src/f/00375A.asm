; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Players - Op2_BranchTouchBox's g_PlayerBoxList case (kinds 0 and 4): Hitbox_FindPlayerBox on the prepared rectangle; also holds the shared not-found ($3760) and found ($376E) exits
; In:  d4-d7 = rectangle, a0 = the op's offset word
; Out: found: g_FoundBox = box, branch via the offset; else a2 = a0 + 2, NEXT
; Evidence: bsr Hitbox_FindPlayerBox / bne $376E (move.l a2,g_FoundBox / adda.w); reached from the jump table at $374A/$374E; 114 runs
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Players:
	bsr.w	Hitbox_FindPlayerBox
	bne.s	Op2_BranchTouchBox_Hit
; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Miss - Op2_BranchTouchBox's shared not-found exit: the script goes on after the offset word
; In:  a0 = the op's offset word
; Out: a2 = a0 + 2; NEXT
; Evidence: lea $2(a0),a2 / NEXT; the bne fall-through of Op2_BranchTouchBox_Players and the beq target of Op2_BranchTouchBox_Enemies
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Miss:
	lea.l	$2(a0),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Enemies - Op2_BranchTouchBox's g_EnemyBoxList case (kind 8): Hitbox_FindEnemyBox on the prepared rectangle, then the shared exits
; In:  d4-d7 = rectangle, a0 = the op's offset word
; Out: as Op2_BranchTouchBox_Players
; Evidence: bsr Hitbox_FindEnemyBox / beq $3760; reached from $3752; 86 runs
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Enemies:
	bsr.w	Hitbox_FindEnemyBox
	beq.s	Op2_BranchTouchBox_Miss
; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Hit - Op2_BranchTouchBox's shared found exit: remembers the box in g_FoundBox and takes the branch
; In:  a2 = the box found; a0 = the op's offset word
; Out: g_FoundBox = the box; a2 = the offset word + its value; NEXT
; Evidence: move.l a2,(g_FoundBox).w / movea.l a0,a2 / adda.w (a2),a2 / NEXT; the bne target of Op2_BranchTouchBox_Players and the fall-through of Op2_BranchTouchBox_Enemies
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Hit:
	move.l	a2,(g_FoundBox).w
	movea.l	a0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetAttackBoxes - a2 op: replaces the object's attack boxes (a4 +$A count, +$C handles) with n boxes described inline (Obj_SetAttackBoxes)
; In:  a2 = script: word n, then n box specs (see Obj_SetAttackBoxes)
; Out: NEXT after the specs
; Evidence: move.w (a2)+,d7 / bsr Obj_SetAttackBoxes; 3 runs; $1C53E `377A 0002 0004 FFEB FFFD 0015 001B 0004 0001 0614 0030 ...`
; ---------------------------------------------------------------------------
Op2_SetAttackBoxes:
	move.w	(a2)+,d7
	bsr.w	Obj_SetAttackBoxes
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetAttackBox - a2 op: replaces the object's attack boxes with one box described inline (the usual punch/kick hit box with its hit callback)
; In:  a2 = script: one box spec (see Obj_SetAttackBoxes)
; Out: NEXT after the spec
; Evidence: moveq #1,d7 / bsr Obj_SetAttackBoxes; 105 runs; $18106 `3784 0004 FFE2 FFEC 001E 0032 0004 000105C6 0020 0300` (callback $105C6, damage $20, knockback $300: NOTES 'Weapons and hits')
; ---------------------------------------------------------------------------
Op2_SetAttackBox:
	moveq	#$1,d7
	bsr.w	Obj_SetAttackBoxes
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearAttackBoxes - a2 op: removes all of the object's attack boxes (frees their pool blocks)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: moveq #0,d7 / bsr Obj_SetAttackBoxes (frees every old handle); 90 runs
; ---------------------------------------------------------------------------
Op2_ClearAttackBoxes:
	moveq	#$0,d7
	bsr.w	Obj_SetAttackBoxes
	movea.w	(a2)+,a0
	jmp	(a0)
