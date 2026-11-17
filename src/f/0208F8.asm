; ---------------------------------------------------------------------------
; Stage1_InitSignDebris - initializes one $18-byte sign-debris piece from a 5-word Tbl_02095E record: frame base, x/y offsets from the burst, x and y launch speeds plus random spread, random animation rate
; In:  a3 = record (frame, dx, dy, vx, vy), a5 = piece, d4/d5 = centre, d6 = attr
; Out: piece +0 vx, +4 x, +8 vy, +$C y (16.16), +$10 rate $80-$BF, +$12 anim 0, +$14 frame, +$16 attr; a3 += 10, a5 += $18
; Evidence: Rand_Next three times; vy = -(rand*6) + vy<<8; called 18 times by Stage1_SpawnSignDebris
; ---------------------------------------------------------------------------
Stage1_InitSignDebris:
	move.w	(a3)+,$14(a5)
	jsr	(Rand_Next).l
	andi.b	#$3F,d0
	addi.b	#$80,d0
	move.b	d0,$10(a5)
	clr.w	$12(a5)
	move.w	(a3)+,d0
	add.w	d4,d0
	move.w	d0,$4(a5)
	move.w	(a3)+,d0
	add.w	d5,d0
	move.w	d0,$C(a5)
	jsr	(Rand_Next).l
	move.w	(a3)+,d1
	ext.l	d1
	asl.l	#$8,d1
	add.l	d1,d0
	move.l	d0,(a5)
	jsr	(Rand_Next).l
	andi.l	#$FFFF,d0
	add.l	d0,d0
	move.l	d0,d1
	add.l	d0,d0
	add.l	d1,d0
	neg.l	d0
	move.w	(a3)+,d1
	ext.l	d1
	asl.l	#$8,d1
	add.l	d1,d0
	move.l	d0,$8(a5)
	move.w	d6,$16(a5)
	lea.l	$18(a5),a5
	rts
