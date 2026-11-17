; ---------------------------------------------------------------------------
; VM4_CalcDrawPos - computes an a4 object's drawn point: d4/d5 = (x, y) + radius * (cos, sin)(heading), 16.16
; In:  a6 = object (+$26 radius, +$2A heading, +$2E/+$32 x/y)
; Out: d4/d5 = drawn x/y (16.16); d0/d1/a0 used
; Evidence: movem.l $2E(a6),d4-d5; radius word * Tbl_059B22 sine (cos at +$100) by heading & $3FE; callers Op4_SpawnAtDrawPos, Op4_CalcDrawPos
; ---------------------------------------------------------------------------
VM4_CalcDrawPos:
	movem.l	$2E(a6),d4-d5
	move.w	$26(a6),d0
	beq.s	loc_006274
	lea.l	(Tbl_059B22).l,a0
	move.w	$2A(a6),d1
	andi.w	#$3FE,d1
	adda.w	d1,a0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	add.l	d0,d4
	add.l	d1,d5
loc_006274:
	rts
; ---------------------------------------------------------------------------
; Op4_BounceY - a4 op: reverses and halves the object's y velocity $42 (a bounce)
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: asr.l #1 / neg.l on $42(a6); script use $1F216 (`6276 549A`); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_BounceY:
	move.l	$42(a6),d0
	asr.l	#$1,d0
	neg.l	d0
	move.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_Move - a4 op: steps the object's full motion (each velocity += its acceleration, radius/heading/x/y += velocity) and leaves the drawn point's pixel words in d4/d5
; In:  a4 = script (no operands); a6 = object
; Out: NEXT; d4/d5 = drawn x/y words
; Evidence: movem $36-$42 += $46-$52, added to $26-$32, then the drawn point (Tbl_059B22) swapped; 97 script uses, e.g. $1323A (`6286 65B4`), $136E8; 137 corpus runs
; ---------------------------------------------------------------------------
Op4_Move:
	movem.l	$36(a6),d0-d1/d6-d7
	add.l	$46(a6),d0
	add.l	$4A(a6),d1
	add.l	$4E(a6),d6
	add.l	$52(a6),d7
	movem.l	d0-d1/d6-d7,$36(a6)
	add.l	d0,$26(a6)
	add.l	d1,$2A(a6)
	add.l	d6,$2E(a6)
	add.l	d7,$32(a6)
	movem.l	$2E(a6),d4-d5
	move.w	$26(a6),d0
	beq.s	loc_0062DA
	lea.l	(Tbl_059B22).l,a0
	move.w	$2A(a6),d1
	andi.w	#$3FE,d1
	adda.w	d1,a0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	add.l	d0,d4
	add.l	d1,d5
loc_0062DA:
	swap	d4
	swap	d5
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_MovePolar - a4 op: steps only the radius and heading motion (velocity += acceleration, value += velocity) and leaves the drawn point's pixel words (x/y words + radius * (cos, sin)) in d4/d5
; In:  a4 = script (no operands)
; Out: NEXT; d4/d5 = drawn x/y words
; Evidence: updates $36/$26 and $3A/$2A only; script uses $20FF6 (`62E2 67EE`), shot scripts $50548 (`62E2 69A8`); 151 corpus runs
; ---------------------------------------------------------------------------
Op4_MovePolar:
	move.l	$36(a6),d0
	add.l	$46(a6),d0
	move.l	d0,$36(a6)
	add.l	d0,$26(a6)
	move.l	$3A(a6),d0
	add.l	$4A(a6),d0
	move.l	d0,$3A(a6)
	add.l	d0,$2A(a6)
	move.w	$2A(a6),d0
	andi.w	#$3FE,d0
	lea.l	(Tbl_059B22).l,a0
	adda.w	d0,a0
	move.w	$26(a6),d4
	move.w	d4,d5
	muls.w	$100(a0),d4
	muls.w	(a0),d5
	swap	d4
	swap	d5
	add.w	$2E(a6),d4
	add.w	$32(a6),d5
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_MoveXY - a4 op: steps only the x/y motion (velocity += acceleration, x/y += velocity) and leaves the x/y pixel words in d4/d5 (radius and heading ignored); Op4_LoadXY is its tail
; In:  a4 = script (no operands)
; Out: NEXT; d4/d5 = x/y words
; Evidence: updates $3E/$2E and $42/$32; script uses $10DC6 (`632E 67EE`), $15150; 143 corpus runs
; ---------------------------------------------------------------------------
Op4_MoveXY:
	move.l	$3E(a6),d0
	add.l	$4E(a6),d0
	move.l	d0,$3E(a6)
	add.l	d0,$2E(a6)
	move.l	$42(a6),d0
	add.l	$52(a6),d0
	move.l	d0,$42(a6)
	add.l	d0,$32(a6)
; ---------------------------------------------------------------------------
; Op4_LoadXY - a4 op: loads the object's x/y pixel words into d4/d5 without moving it (a still object's per-frame step before drawing)
; In:  a4 = script (no operands)
; Out: NEXT; d4/d5 = x/y words
; Evidence: dispatched entry inside Op4_MoveXY (computed-jump target in 151 corpus runs); 22 script uses, e.g. $108E0 (`634E 67EE`)
; ---------------------------------------------------------------------------
Op4_LoadXY:
	move.w	$2E(a6),d4
	move.w	$32(a6),d5
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_PolarToXY - a4 op: folds the polar part into x/y: adds radius * (cos, sin)(heading) to x/y and radius velocity * (cos, sin) to the x/y velocities, then clears radius, its velocity and acceleration
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: Tbl_059B22 by $2A; adds to $2E/$32 and $3E/$42; clr.l $26/$36/$46; script uses $15142 (`635A 6418`), $26536; 63 corpus runs
; ---------------------------------------------------------------------------
Op4_PolarToXY:
	lea.l	(Tbl_059B22).l,a0
	move.w	$2A(a6),d0
	andi.w	#$3FE,d0
	adda.w	d0,a0
	move.w	$26(a6),d0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	add.l	d0,$2E(a6)
	add.l	d1,$32(a6)
	move.w	$36(a6),d0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	add.l	d0,$3E(a6)
	add.l	d1,$42(a6)
	clr.l	$26(a6)
	clr.l	$36(a6)
	clr.l	$46(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SpawnFromSelf - a4 op: loads the spawn registers d2-d5 with the object's own radius, heading, x and y (16.16), so the next spawn starts on it
; In:  a4 = script (no operands)
; Out: NEXT; d2-d5 = $26-$32(a6)
; Evidence: movem.l $26(a6),d2-d5; 41 script uses, e.g. $14E48 (`63A2 5606`: then Op4_Spawn); 76 corpus runs
; ---------------------------------------------------------------------------
Op4_SpawnFromSelf:
	movem.l	$26(a6),d2-d5
	movea.w	(a4)+,a0
	jmp	(a0)
