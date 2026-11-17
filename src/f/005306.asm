; ---------------------------------------------------------------------------
; Op2_FxHeading - a2 op: sets the effect-spawn heading d3 to a word (0-$3FF, $400 a turn) in the high word
; In:  a2 = script: word heading
; Out: d3 = heading << 16; NEXT
; Evidence: move.w (a2)+,d3 / swap d3; 16 corpus runs; $1C2CA `5306 0300 5198`
; ---------------------------------------------------------------------------
Op2_FxHeading:
	move.w	(a2)+,d3
	swap	d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxHeadingByFacing - a2 op: sets the effect-spawn heading d3 from a pair by the sprite's facing: the first word when H-flipped (facing left), the second otherwise
; In:  a2 = script: word heading if flipped, word heading if not
; Out: d3 = heading << 16; NEXT
; Evidence: move.l (a2)+,d3 / btst #3,$38(a5) / bne / swap / clr.w d3; 18 corpus runs; $258FC `530E 0180 0080` ($180 down-left / $80 down-right)
; ---------------------------------------------------------------------------
Op2_FxHeadingByFacing:
	move.l	(a2)+,d3
	btst	#$3,$38(a5)
	bne.s	loc_00531A
	swap	d3
loc_00531A:
	clr.w	d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxHeadingSnap - a2 op: sets the effect-spawn heading d3 to the object's heading $C(a3) rounded to the nearest of n directions
; In:  a2 = script: word n
; Out: d3 = round(heading * n / 1024) * ($400 / n) << 16; NEXT
; Evidence: divu.w d3 of $40000 / mulu.w $C(a3),d3 / addi.l #$200 / lsr #8, #2 / mulu.w d0 / lsl.l #8; 14 corpus runs; $2FB20 `5320 0010 5198`
; ---------------------------------------------------------------------------
Op2_FxHeadingSnap:
	move.w	(a2)+,d3
	move.l	#$40000,d0
	divu.w	d3,d0
	mulu.w	$C(a3),d3
	addi.l	#$200,d3
	lsr.l	#$8,d3
	lsr.w	#$2,d3
	mulu.w	d0,d3
	lsl.l	#$8,d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxHeadingSnapClamp - a2 op: as Op2_FxHeadingSnap with the direction index clamped to [lo, hi]
; In:  a2 = script: word n, word lo, word hi
; Out: d3 = clamped direction * ($400 / n) << 16; NEXT
; Evidence: cmp.w (a2)+,d3 / bge / move.w -2(a2),d3 twice; 6 corpus runs; $26344 `5340 0028 0005 000F`
; ---------------------------------------------------------------------------
Op2_FxHeadingSnapClamp:
	move.w	(a2)+,d3
	move.l	#$40000,d0
	divu.w	d3,d0
	mulu.w	$C(a3),d3
	addi.l	#$200,d3
	lsr.l	#$8,d3
	lsr.w	#$2,d3
	cmp.w	(a2)+,d3
	bge.s	loc_005360
	move.w	-$2(a2),d3
loc_005360:
	cmp.w	(a2)+,d3
	ble.s	loc_005368
	move.w	-$2(a2),d3
loc_005368:
	mulu.w	d0,d3
	lsl.l	#$8,d3
	movea.w	(a2)+,a0
	jmp	(a0)
