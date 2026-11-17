; ---------------------------------------------------------------------------
; Op4_SaveDrawPos - a4 op: computes the object's drawn point ((x, y) + radius * (cos, sin)(heading)) and stores its pixel words in $56/$58(a6)
; In:  a4 = script (no operand); a6 = object; the heading is read from a5 (= a6 unless a child was spawned this run, inferred quirk)
; Out: NEXT; $56/$58(a6) = drawn x/y
; Evidence: movem $2E/$32, radius $26 * Tbl_059B22 entries +$100 (cos) / +0 (sin) at $2A(a5), movem.w d6-d7,$56(a6); script uses $1F200, $26BE6, $26D4C; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_SaveDrawPos:
	movem.l	$2E(a6),d6-d7
	move.w	$26(a6),d0
	beq.s	loc_0057E8
	lea.l	(Tbl_059B22).l,a0
	move.w	$2A(a5),d1
	andi.w	#$3FE,d1
	adda.w	d1,a0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	add.l	d0,d6
	add.l	d1,d7
loc_0057E8:
	swap	d6
	swap	d7
	movem.w	d6-d7,$56(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSaveDrawPos - a4 op: computes the last spawned child's (a5) drawn point from its x/y/radius/heading and stores it in the child's $56/$58
; In:  a4 = script (no operand); a5 = child
; Out: NEXT
; Evidence: the Op4_SaveDrawPos code on a5 throughout; 38 a4-script uses, e.g. $1A394, $1A640 (after the child's velocity ops, before the next spawn); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSaveDrawPos:
	movem.l	$2E(a5),d6-d7
	move.w	$26(a5),d0
	beq.s	loc_00581E
	lea.l	(Tbl_059B22).l,a0
	move.w	$2A(a5),d1
	andi.w	#$3FE,d1
	adda.w	d1,a0
	move.w	d0,d1
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	add.l	d0,d6
	add.l	d1,d7
loc_00581E:
	swap	d6
	swap	d7
	movem.w	d6-d7,$56(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
