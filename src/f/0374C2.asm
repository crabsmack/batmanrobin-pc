; ---------------------------------------------------------------------------
; ToyChain_PlaceLinkBoxes - when var 0 bit 1 is set, gives every link a 16x16 body box around its position with the chain's energy and links it into the enemy box list (shootable links)
; In:  a2 = chain block, a6 = chain object, a4 +$22 var 0, +$24 count
; Out: each link box: +$1E energy, +$12-$18 rect, linked by Hitbox_LinkEnemyList when not yet linked
; Evidence: andi #$2; subq/addq #8 around $44/$46; movem.w d4-d7,$12(a0); TAS 6579 frames
; ---------------------------------------------------------------------------
ToyChain_PlaceLinkBoxes:
	move.w	$22(a4),d0
	andi.w	#$2,d0
	beq.s	loc_03750A
	move.w	$24(a4),d3
	subq.w	#$1,d3
	bmi.s	loc_03750A
	lea.l	$34(a2),a1
loc_0374D8:
	movea.w	(a1)+,a0
	move.w	$52(a6),$1E(a0)
	move.w	$44(a0),d4
	move.w	$46(a0),d5
	move.w	d4,d6
	move.w	d5,d7
	subq.w	#$8,d4
	subq.w	#$8,d5
	addq.w	#$8,d6
	addq.w	#$8,d7
	movem.w	d4-d7,$12(a0)
	tst.w	$2(a0)
	bne.s	loc_037506
	jsr	(Hitbox_LinkEnemyList).l
loc_037506:
	dbra	d3,loc_0374D8
loc_03750A:
	rts
