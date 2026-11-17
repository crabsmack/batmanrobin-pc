; ---------------------------------------------------------------------------
; Op2_MoveTerrain - a2 op: moves the object by d1/d2 through the scene terrain: without terrain boxes it just adds the move; box B (bit 2 of (a3)) is collided first (wall/floor results $52/$54, d3 -> $50); if B finds no floor and box A (bit 1) is on, box A is collided, clearing the x/y velocity on wall/floor contact
; In:  a2 = script (no operand); d1/d2 = move (16.16); a3 = motion block (boxes A +$40-$46, B +$48-$4E)
; Out: $3E/$42(a6) moved (fractions $40/$44 kept); $50/$52/$54(a3) results; NEXT
; Evidence: jsr Coll_MoveScene with d4/d5 = position + box offset, d6/d7 = masks; clr.l $18(a3)/$1C(a3) on contact; 102 corpus runs; $133A8 `4B74 4CDA`
; ---------------------------------------------------------------------------
Op2_MoveTerrain:
	movem.l	$3E(a6),d4-d5
	add.l	d4,d1
	add.l	d5,d2
	move.w	d1,$40(a6)
	move.w	d2,$44(a6)
	swap	d1
	swap	d2
	swap	d4
	swap	d5
	sub.w	d4,d1
	sub.w	d5,d2
	btst	#$2,(a3)
	bne.s	loc_004D14
	btst	#$1,(a3)
	bne.s	loc_004D58
	add.w	d1,$3E(a6)
	add.w	d2,$42(a6)
	clr.w	$50(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004D14:
	add.w	$48(a3),d4
	add.w	$4A(a3),d5
	movem.w	$4C(a3),d6-d7
	jsr	(Coll_MoveScene).l
	move.w	d6,$52(a3)
	move.w	d7,$54(a3)
	beq.s	loc_004D4A
loc_004D32:
	sub.w	$48(a3),d4
	sub.w	$4A(a3),d5
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	move.w	d3,$50(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004D4A:
	btst	#$1,(a3)
	beq.s	loc_004D32
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
loc_004D58:
	add.w	$40(a3),d4
	add.w	$42(a3),d5
	movem.w	$44(a3),d6-d7
	jsr	(Coll_MoveScene).l
	sub.w	$40(a3),d4
	sub.w	$42(a3),d5
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	movem.w	d3,$50(a3)
	move.w	d6,$52(a3)
	beq.s	loc_004D8C
	clr.l	$18(a3)
loc_004D8C:
	move.w	d7,$54(a3)
	beq.w	VM2_Next
	clr.l	$1C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SubLiftVelY - a2 op: subtracts the lift section's vertical speed g_LiftVelY from this frame's y move d2 (moves an object relative to the lift)
; In:  a2 = script (no operand); d2 = y move
; Out: d2 -= g_LiftVelY; NEXT
; Evidence: sub.l ($E006),d2; g_LiftVelY is ramped by $2410A; used in the zeppelin-dock scripts $33C10 `4D9C 4C52 3E58`; 20 corpus runs
; ---------------------------------------------------------------------------
Op2_SubLiftVelY:
	sub.l	(g_LiftVelY).w,d2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TestAttackBoxes - a2 op: tests every attack box of the object against its target list (Hitbox_TestAttackBox), unless the attack pause count $1(a4) is running, which it counts down instead
; In:  a2 = script (no operand); a4 +$A count / +$C handles of the attack boxes
; Out: hits handled by the box callbacks; NEXT
; Evidence: tst.b $1(a4) / subq.b; movea.w $C(a4),a0 / bsr Hitbox_TestAttackBox (loop over $C(a4).. for more); PlayerAtk_HitPause sets $1(a4); 114 corpus runs; $1812E `4DA4 448A`
; ---------------------------------------------------------------------------
Op2_TestAttackBoxes:
	tst.b	$1(a4)
	beq.s	loc_004DB2
	subq.b	#$1,$1(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004DB2:
	move.w	$A(a4),d7
	subq.w	#$1,d7
	bmi.w	VM2_Next
	bne.s	loc_004DCE
	move.l	a2,-(a7)
	movea.w	$C(a4),a0
	bsr.w	Hitbox_TestAttackBox
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004DCE:
	pea.l	(a2)
	lea.l	$C(a4),a1
loc_004DD4:
	movea.w	(a1)+,a0
	movem.l	d7/a1,-(a7)
	bsr.w	Hitbox_TestAttackBox
	movem.l	(a7)+,d7/a1
	dbra	d7,loc_004DD4
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
