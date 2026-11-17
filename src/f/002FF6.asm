; ---------------------------------------------------------------------------
; Op2_HoldRangeOfPlayerX - a2 op: walks to stay a horizontal distance [near, far] (+ jitter $1C(a6)) from the targeted player on the side the object is on, in screen space, facing the player once there; a window pushed past the screen edge ($136/$A) is clamped, and when the object is within margin of the player's far side it goes round to the other side; stopped objects widen the window by 6 (hysteresis)
; In:  a2 = script: words near, far, margin, word offset
; Out: in range: x speed 0, faces the player, branch via the offset; moving: x target speed +-cruise, facing the way it walks, NEXT after the offset; no target: brakes x and continues after the operands
; Evidence: movem.w (a2)+,d2-d4; positions minus g_CameraX; cmpi #$136/#$12C, #$A/#$14 clamps; subq/addq #6 when $24(a3) = 0; 31 runs; $1B0B2 `2FF6 006E 0078 0008 FF6C`
; ---------------------------------------------------------------------------
Op2_HoldRangeOfPlayerX:
	move.w	$10(a4),d0
	beq.w	loc_003006
	movea.w	d0,a0
	move.w	$2E(a0),d5
	bra.s	loc_003026
loc_003006:
	addq.l	#$8,a2
	bra.w	Op2_ClearTargetSpeedX
; ---------------------------------------------------------------------------
; Op2_HoldRangeOfPlayerXTurnBranch - a2 op: as Op2_HoldRangeOfPlayerX but leaves the facing alone and branches on it instead: in range and facing the player -> offset 0, needing to face left -> offset 1, needing to face right -> offset 2; walking forward continues
; In:  a2 = script: words near, far, margin, words offset0, offset1, offset2
; Out: a branch, or NEXT after the operands (also with no target, x braked)
; Evidence: same window code as $2FF6 with btst #3,$38(a5) choosing Op2_Goto / $314C (skip 1) / $3196 (skip 2) / $31AC (skip 3); 5 runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_HoldRangeOfPlayerXTurnBranch:
	move.w	$10(a4),d0
	beq.w	loc_00301E
	movea.w	d0,a0
	move.w	$2E(a0),d5
	bra.w	loc_0030F0
loc_00301E:
	lea.l	$C(a2),a2
	bra.w	Op2_ClearTargetSpeedX
loc_003026:
	movem.w	(a2)+,d2-d4
	add.w	$1C(a6),d2
	add.w	$1C(a6),d3
	move.w	$3E(a6),d6
	sub.w	(g_CameraX).w,d5
	sub.w	(g_CameraX).w,d6
	cmp.w	d6,d5
	bgt.s	loc_003084
	add.w	d5,d2
	add.w	d5,d3
	cmpi.w	#$136,d3
	ble.s	loc_003062
	move.w	#$136,d3
	cmpi.w	#$12C,d2
	ble.s	loc_003062
	move.w	#$12C,d2
	add.w	d5,d4
	sub.w	d6,d4
	bgt.w	loc_0030DA
loc_003062:
	tst.w	$24(a3)
	bne.s	loc_00306C
	subq.w	#$6,d2
	addq.w	#$6,d3
loc_00306C:
	cmp.w	d6,d3
	blt.s	loc_0030DA
	cmp.w	d6,d2
	bgt.s	loc_0030C8
	bset	#$3,$38(a5)
	clr.l	$24(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_003084:
	neg.w	d2
	neg.w	d3
	add.w	d5,d2
	add.w	d5,d3
	cmpi.w	#$A,d3
	bge.s	loc_0030A6
	move.w	#$A,d3
	cmpi.w	#$14,d2
	bge.s	loc_0030A6
	move.w	#$14,d2
	add.w	d6,d4
	sub.w	d5,d4
	bgt.s	loc_0030C8
loc_0030A6:
	tst.w	$24(a3)
	bne.s	loc_0030B0
	subq.w	#$6,d3
	addq.w	#$6,d2
loc_0030B0:
	cmp.w	d6,d2
	blt.s	loc_0030DA
	cmp.w	d6,d3
	bgt.s	loc_0030C8
	bclr	#$3,$38(a5)
	clr.l	$24(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0030C8:
	move.l	$30(a3),$24(a3)
	bclr	#$3,$38(a5)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0030DA:
	move.l	$30(a3),d0
	neg.l	d0
	move.l	d0,$24(a3)
	bset	#$3,$38(a5)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0030F0:
	movem.w	(a2)+,d2-d4
	add.w	$1C(a6),d2
	add.w	$1C(a6),d3
	move.w	$3E(a6),d6
	sub.w	(g_CameraX).w,d5
	sub.w	(g_CameraX).w,d6
	cmp.w	d6,d5
	bgt.s	loc_003154
	add.w	d5,d2
	add.w	d5,d3
	cmpi.w	#$136,d3
	ble.s	loc_00312C
	move.w	#$136,d3
	cmpi.w	#$12C,d2
	ble.s	loc_00312C
	move.w	#$12C,d2
	add.w	d5,d4
	sub.w	d6,d4
	bgt.w	loc_0031B2
loc_00312C:
	tst.w	$24(a3)
	bne.s	loc_003136
	subq.w	#$6,d2
	addq.w	#$6,d3
loc_003136:
	cmp.w	d6,d3
	blt.s	loc_0031B2
	cmp.w	d6,d2
	bgt.s	loc_00319E
	clr.l	$24(a3)
	btst	#$3,$38(a5)
	bne.w	Op2_Goto
loc_00314C:
	addq.l	#$2,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_003154:
	neg.w	d2
	neg.w	d3
	add.w	d5,d2
	add.w	d5,d3
	cmpi.w	#$A,d3
	bge.s	loc_003176
	move.w	#$A,d3
	cmpi.w	#$14,d2
	bge.s	loc_003176
	move.w	#$14,d2
	add.w	d6,d4
	sub.w	d5,d4
	bgt.s	loc_00319E
loc_003176:
	tst.w	$24(a3)
	bne.s	loc_003180
	subq.w	#$6,d3
	addq.w	#$6,d2
loc_003180:
	cmp.w	d6,d2
	blt.s	loc_0031B2
	cmp.w	d6,d3
	bgt.s	loc_00319E
	clr.l	$24(a3)
	btst	#$3,$38(a5)
	beq.w	Op2_Goto
loc_003196:
	addq.l	#$4,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00319E:
	move.l	$30(a3),$24(a3)
	btst	#$3,$38(a5)
	bne.s	loc_003196
loc_0031AC:
	addq.l	#$6,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0031B2:
	move.l	$30(a3),d0
	neg.l	d0
	move.l	d0,$24(a3)
	btst	#$3,$38(a5)
	bne.s	loc_0031AC
	bra.s	loc_00314C
