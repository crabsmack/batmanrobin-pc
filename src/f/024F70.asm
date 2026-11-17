; ---------------------------------------------------------------------------
; Stage2_MoveOnScrollingMap - a2 native move step for objects on the lift shaft's girders: applies the d1/d2 move through Coll_MoveOnBlockMap in map space (y + g_MapScrollY), with a 2-px downward probe when not moving vertically, and stores the terrain results like the move op
; In:  a3 = motion block (+$40/$42 box offset, +$44/$46 masks), a6 = object, d1/d2 = move (16.16)
; Out: x/y updated; $50/$52/$54(a3) = d3/wall/floor; wall clears the x speed, floor the y speed and target
; Evidence: Op2_CallNativeRel at $024D2A after Op2_StepVelocity (barrel thug coroutine 1); TAS f22331-f31862 (1590 frames)
; ---------------------------------------------------------------------------
Stage2_MoveOnScrollingMap:
	tst.l	d2
	bne.s	loc_024F7A
	addi.l	#$20000,d2
loc_024F7A:
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
	add.w	$40(a3),d4
	add.w	$42(a3),d5
	add.w	(g_MapScrollY).w,d5
	movem.w	$44(a3),d6-d7
	jsr	(Coll_MoveOnBlockMap).l
	sub.w	$40(a3),d4
	sub.w	$42(a3),d5
	sub.w	(g_MapScrollY).w,d5
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	movem.w	d3,$50(a3)
	move.w	d6,$52(a3)
	beq.s	loc_024FD4
	clr.l	$18(a3)
loc_024FD4:
	move.w	d7,$54(a3)
	beq.s	loc_024FE2
	clr.l	$1C(a3)
	clr.l	$2C(a3)
loc_024FE2:
	rts
