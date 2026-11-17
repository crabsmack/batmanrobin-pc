; ---------------------------------------------------------------------------
; Op2_TestAttackBoxAtDrawn - a2 op: tests one attack box, chosen by index, at the object's drawn position $4E/$50(a6), unless the attack pause count $1(a4) is running
; In:  a2 = script: word index into the attack boxes ($A(a4) count, $C(a4) handles)
; Out: NEXT (an index out of range does nothing)
; Evidence: cmp.w $A(a4),d0 / movea.w $C(a4,d0.w),a0 / move.w $4E(a6),d1 / bsr Hitbox_TestAttackBoxAt; 5 corpus runs; $1E7BE `4EA8 0000`
; ---------------------------------------------------------------------------
Op2_TestAttackBoxAtDrawn:
	move.w	(a2)+,d0
	tst.b	$1(a4)
	beq.s	loc_004EB8
	subq.b	#$1,$1(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004EB8:
	cmp.w	$A(a4),d0
	bge.w	VM2_Next
	add.w	d0,d0
	movea.w	$C(a4,d0.w),a0
	pea.l	(a2)
	move.w	$4E(a6),d1
	move.w	$50(a6),d2
	bsr.w	Hitbox_TestAttackBoxAt
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlaceBodyBoxes - a2 op: places every body box for this frame: copies the owner's energy $52(a6) to box +$1E, writes the absolute rectangle +$12-$18 (mirrored by the flip bits) and links boxes not yet on a list through Hitbox_LinkByKind; while the off count (a4) runs it unlinks them instead
; In:  a2 = script (no operand); a4 +$2 count / +$4 handles of the body boxes (+6 kind, +8.. rect)
; Out: NEXT
; Evidence: movem.w d4-d7,$12(a0) / tst.w $2(a0) / jsr $4F3E(pc,d3.w); tst.b (a4) -> Op2_PlaceBodyBoxes_Off; 151 corpus runs; $1812C `4EDA 4DA4 448A`
; ---------------------------------------------------------------------------
Op2_PlaceBodyBoxes:
	move.w	$2(a4),d0
	subq.w	#$1,d0
	bmi.w	VM2_Next
	lea.l	$4(a4),a1
	tst.b	(a4)
	bne.s	Op2_PlaceBodyBoxes_Off
	move.w	$3E(a6),d1
	move.w	$42(a6),d2
; ---------------------------------------------------------------------------
; Op2_PlaceBodyBoxes_Next - loop head of Op2_PlaceBodyBoxes: takes the next body-box handle from a1 and places that box
; In:  a1 = the next handle; d0 = boxes left - 1; d1/d2 = position
; Out: as Op2_PlaceBodyBoxes_Place
; Evidence: movea.w (a1)+,a0 then the box placement; the dbra target of the placement loop
; ---------------------------------------------------------------------------
Op2_PlaceBodyBoxes_Next:
	movea.w	(a1)+,a0
; ---------------------------------------------------------------------------
; Op2_PlaceBodyBoxes_Place - places one body box a0 at d1/d2 (energy copy, mirrored absolute rectangle, link by kind when not on a list), then loops over the rest of the handles
; In:  a0 = box; d1/d2 = position; d0 = boxes left - 1; a1 = the next handle
; Out: NEXT after the loop
; Evidence: move.w $52(a6),$1E(a0) / movem.w $6(a0),d3-d7 / movem.w d4-d7,$12(a0) / jsr Hitbox_LinkByKind(pc,d3.w); beq.w here from Op2_PlaceBodyBox / Op2_PlaceBodyBoxAtDrawn
; ---------------------------------------------------------------------------
Op2_PlaceBodyBoxes_Place:
	move.w	$52(a6),$1E(a0)
	movem.w	$6(a0),d3-d7
	btst	#$3,$38(a5)
	beq.s	loc_004F10
	neg.w	d4
	neg.w	d6
	exg.l	d4,d6
loc_004F10:
	btst	#$4,$38(a5)
	beq.s	loc_004F1E
	neg.w	d5
	neg.w	d7
	exg.l	d5,d7
loc_004F1E:
	add.w	d1,d4
	add.w	d1,d6
	add.w	d2,d5
	add.w	d2,d7
	movem.w	d4-d7,$12(a0)
	tst.w	$2(a0)
	bne.s	loc_004F36
	jsr	Hitbox_LinkByKind(pc,d3.w)
loc_004F36:
	dbra	d0,Op2_PlaceBodyBoxes_Next
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Hitbox_LinkByKind - jump table by a box's kind word: kinds 0/4 append the box to g_PlayerBoxList ($8C3C), kinds 8/$C to g_EnemyBoxList ($8C54)
; In:  a0 = box; d3 = kind (byte offset into the table)
; Out: box linked (rts of the append routine)
; Evidence: jsr $4F3E(pc,d3.w) in Op2_PlaceBodyBoxes; bra $8C3C at $4F3E/$4F42, bra $8C54 at $4F46/$4F4A; $8C3C/$8C54 write $DEB2/$DEBA (the lists' tails)
; ---------------------------------------------------------------------------
Hitbox_LinkByKind:
	bra.w	Hitbox_LinkPlayerList
