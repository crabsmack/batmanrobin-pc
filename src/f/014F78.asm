; ---------------------------------------------------------------------------
; Mot_DirToPlayer - picks which of d4 compass directions points from (d2, d3) to the player record a0 (its x +$2E, y +$32 + 16): 0 = up, d4/4 = right, clockwise
; In:  a0 = targeted player record ($10(a4)), d2/d3 = from x/y, d4 = number of directions
; Out: d2 = direction 0..d4-1 (rounded, wraps)
; Evidence: base = d4*32 per octant, ratio minor*base/major, (sum + $80) >> 8, >= d4 wraps; the a2 op at $3C66 stores 4*result to the frame select $1A(a5); Boss_Harley_AimAtPlayer (24 dirs), Boss_Harley_PickAttackCase (8 dirs); TAS f1571-f15702
; ---------------------------------------------------------------------------
Mot_DirToPlayer:
	move.w	$2E(a0),d0
	move.w	$32(a0),d1
	addi.w	#$10,d1
	sub.w	d2,d0
	bne.s	loc_014F8A
	moveq	#$1,d0
loc_014F8A:
	sub.w	d3,d1
	bne.s	loc_014F90
	moveq	#$1,d1
loc_014F90:
	move.w	d4,d2
	lsl.w	#$5,d2
	tst.w	d0
	bmi.s	loc_014FCE
	tst.w	d1
	bmi.s	loc_014FB6
	cmp.w	d1,d0
	bmi.s	loc_014FAA
	mulu.w	d2,d1
	divu.w	d0,d1
	add.w	d2,d2
	add.w	d1,d2
	bra.s	loc_01500E
loc_014FAA:
	mulu.w	d2,d0
	divu.w	d1,d0
	add.w	d2,d2
	add.w	d2,d2
	sub.w	d0,d2
	bra.s	loc_01500E
loc_014FB6:
	neg.w	d1
	cmp.w	d1,d0
	bmi.s	loc_014FC6
	mulu.w	d2,d1
	divu.w	d0,d1
	add.w	d2,d2
	sub.w	d1,d2
	bra.s	loc_01500E
loc_014FC6:
	mulu.w	d2,d0
	divu.w	d1,d0
	move.w	d0,d2
	bra.s	loc_01500E
loc_014FCE:
	neg.w	d0
	tst.w	d1
	bmi.s	loc_014FF2
	cmp.w	d1,d0
	bmi.s	loc_014FE6
	mulu.w	d2,d1
	divu.w	d0,d1
	add.w	d2,d2
	add.w	d2,d1
	lsl.w	#$2,d2
	sub.w	d1,d2
	bra.s	loc_01500E
loc_014FE6:
	mulu.w	d2,d0
	divu.w	d1,d0
	add.w	d2,d2
	add.w	d2,d2
	add.w	d0,d2
	bra.s	loc_01500E
loc_014FF2:
	neg.w	d1
	cmp.w	d1,d0
	bmi.s	loc_015006
	mulu.w	d2,d1
	divu.w	d0,d1
	add.w	d2,d2
	add.w	d2,d1
	add.w	d2,d2
	add.w	d1,d2
	bra.s	loc_01500E
loc_015006:
	mulu.w	d2,d0
	divu.w	d1,d0
	lsl.w	#$3,d2
	sub.w	d0,d2
loc_01500E:
	addi.w	#$80,d2
	lsr.w	#$8,d2
	cmp.w	d4,d2
	blt.s	loc_01501A
	sub.w	d4,d2
loc_01501A:
	rts
; ---------------------------------------------------------------------------
; Mot_StepDirToward - turns a direction index one step toward a target the short way round a ring of d1 directions
; In:  d0 = current direction, d2 = target, d1 = number of directions
; Out: d0 = new direction (wrapped to 0..d1-1); unchanged when equal
; Evidence: diff = d2 - d0 compared with +-d1/2 picks +1 or -1, wrap by +-d1; caller Boss_Harley_TurnCannon with 48 directions
; ---------------------------------------------------------------------------
Mot_StepDirToward:
	sub.w	d0,d2
	beq.s	loc_015044
	bpl.s	loc_015034
	move.w	d1,d3
	lsr.w	#$1,d3
	neg.w	d3
	cmp.w	d3,d2
	ble.s	loc_01503C
loc_01502C:
	subq.w	#$1,d0
	bpl.s	loc_015044
	add.w	d1,d0
	rts
loc_015034:
	move.w	d1,d3
	lsr.w	#$1,d3
	cmp.w	d3,d2
	bgt.s	loc_01502C
loc_01503C:
	addq.w	#$1,d0
	cmp.w	d1,d0
	blt.s	loc_015044
	sub.w	d1,d0
loc_015044:
	rts
