; ---------------------------------------------------------------------------
; ToyChain_LinkTouchAttack - when var 0 bit 2 is set (charged), tests half of the links each frame (pattern $22(a5) rotated) against the player boxes and runs the hit: the struck box gets the chain's id, then the link's hit callback and the player box's callback
; In:  a2 = chain block, a6 = chain object, a5 +$22/+$24 touch pattern, a4 +$22 var 0
; Out: player boxes hit (via the callbacks); chain block +$1C/+$18 blink timer and pattern stepped
; Evidence: andi #$4; ror.w $22(a5) with $AAAA from ToyChain_SpawnInline; Hitbox_FindPlayerBox / Hitbox_FindNextBox; jsr $3E(link) then $30(player box); TAS 6579 frames
; ---------------------------------------------------------------------------
ToyChain_LinkTouchAttack:
	move.w	$22(a4),d0
	andi.w	#$4,d0
	beq.s	loc_037586
	move.w	$24(a4),d3
	subq.w	#$1,d3
	bmi.s	loc_037586
	subq.w	#$1,$1C(a2)
	bpl.s	loc_03752E
	move.w	$1E(a2),$1C(a2)
	ror.w	$18(a2)
loc_03752E:
	move.l	a2,-(a7)
	lea.l	$34(a2),a1
	ror.w	$22(a5)
	move.w	$22(a5),$24(a5)
loc_03753E:
	movea.w	(a1)+,a0
	ror.w	$24(a5)
	bcc.s	loc_037580
	move.w	$44(a0),d4
	move.w	$46(a0),d5
	move.w	d4,d6
	move.w	d5,d7
	subq.w	#$8,d4
	subq.w	#$8,d5
	addq.w	#$8,d6
	addq.w	#$8,d7
	jsr	(Hitbox_FindPlayerBox).l
	beq.s	loc_037580
loc_037562:
	move.w	$58(a6),$1A(a2)
	move.l	a1,-(a7)
	movea.l	$3E(a0),a1
	jsr	(a1)
	movea.l	$30(a2),a1
	jsr	(a1)
	movea.l	(a7)+,a1
	jsr	(Hitbox_FindNextBox).l
	bne.s	loc_037562
loc_037580:
	dbra	d3,loc_03753E
	movea.l	(a7)+,a2
loc_037586:
	rts
