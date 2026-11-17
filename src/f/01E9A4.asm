; ---------------------------------------------------------------------------
; Boss_Claw_TickHitPause - claw native (switch): while the hit-pause counter $30(a6) runs, counts it down and returns 1 (the script skips its move), else 0
; In:  a6 = the claw
; Out: d0 = 1 while paused, else 0
; Evidence: op $2D30 (CallNativeRelSwitch) at $1E7A6 in the claw's frame loop; $30 set to 8 by Boss_Claw_HitCallback; TAS f10512-f12925 (1849 frames)
; ---------------------------------------------------------------------------
Boss_Claw_TickHitPause:
	move.w	$30(a6),d0
	beq.s	loc_01E9B0
	subq.w	#$1,$30(a6)
	moveq	#$1,d0
loc_01E9B0:
	rts
; ---------------------------------------------------------------------------
; Boss_Claw_SetSwing - claw native: starts a pendulum swing from three script words: angle (<< 16 into $1E(a6)), restoring acceleration ($22), and the claw's depth (g_Rot3DOuter+$14); velocity $26 = 0
; In:  a2 = script words angle, accel, depth; a6 = the claw
; Out: a2 past the words
; Evidence: op $2CB2 at $1E8C8 ($60, $C000, $100), $1E93E, $1E972 in Scr2_01E87E; Boss_Claw_StepSwing integrates it; TAS only
; ---------------------------------------------------------------------------
Boss_Claw_SetSwing:
	moveq	#$0,d0
	move.w	(a2)+,d0
	swap	d0
	move.l	d0,$1E(a6)
	moveq	#$0,d0
	move.w	(a2)+,d0
	move.l	d0,$22(a6)
	clr.l	$26(a6)
	move.w	(a2)+,(g_Rot3DOuter+$14).l
	rts
; ---------------------------------------------------------------------------
; Boss_Claw_StepSwing - steps the claw's pendulum: velocity $26 += accel ($22) toward angle 0 (negated while the angle $1E is >= 0), angle += velocity; returns the angle
; In:  a6 = the claw
; Out: d0 = the integer angle (high word of $1E)
; Evidence: bsr from Boss_Claw_SwingSideways/SwingAtCrane/SwingFingers; constant-magnitude restoring force, no damping
; ---------------------------------------------------------------------------
Boss_Claw_StepSwing:
	move.l	$22(a6),d2
	move.l	$26(a6),d1
	move.l	$1E(a6),d0
	bmi.s	loc_01E9E0
	neg.l	d2
loc_01E9E0:
	add.l	d2,d1
	add.l	d1,d0
	move.l	d0,$1E(a6)
	move.l	d1,$26(a6)
	swap	d0
	rts
; ---------------------------------------------------------------------------
; Boss_Claw_SwingSideways - claw slot-2 native: swings the whole claw about the third angle (g_Rot3DOuter +8 = swing - $20, other angles 0, fingers' tilt 0), then places the fingers
; In:  a6 = the claw
; Out: -
; Evidence: installed by Op2_StartCoroutine2Rel at $1E97A, run each frame by Op2_CallSlot2Native; 3 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Boss_Claw_SwingSideways:
	bsr.s	Boss_Claw_StepSwing
	lea.l	(g_Rot3DOuter).l,a0
	addi.w	#$FFE0,d0
	move.w	d0,$8(a0)
	clr.w	$4(a0)
	clr.w	(a0)
	lea.l	(g_Rot3DInner).l,a0
	clr.w	$4(a0)
	clr.w	$8(a0)
	bra.w	Boss_Claw_PlaceFingers
; ---------------------------------------------------------------------------
; Boss_Claw_SwingAtCrane - claw slot-2 native: places the fingers once, then makes Boss_Claw_SwingAtCraneStep the slot-2 routine: the claw hanging at the background crane
; In:  a6 = the claw, a4 = its speed block
; Out: $40(a4) = Boss_Claw_SwingAtCraneStep
; Evidence: installed by Op2_StartCoroutine2Rel at $1E946; TAS only; shot f11460 (claw up at the crane tower)
; ---------------------------------------------------------------------------
Boss_Claw_SwingAtCrane:
	bsr.w	Boss_Claw_PlaceFingers
	move.l	#Boss_Claw_SwingAtCraneStep,$40(a4)
; ---------------------------------------------------------------------------
; Boss_Claw_SwingAtCraneStep - per-frame part of Boss_Claw_SwingAtCrane: keeps the claw at x = g_CameraX * 0.6 + $3B8 (it scrolls with the background crane) and swings it about the second angle (g_Rot3DOuter +4, tilt +8 = -$20)
; In:  a6 = the claw
; Out: -
; Evidence: (g_CameraX long >> 8) / $280 = 0.4 x camera subtracted; computed entry at $1EA24 in 3 corpus runs (slot 2 called by Op2_CallSlot2Native)
; ---------------------------------------------------------------------------
Boss_Claw_SwingAtCraneStep:
	move.l	(g_CameraX).w,d0
	lsr.l	#$8,d0
	divu.w	#$280,d0
	neg.w	d0
	addi.w	#$3B8,d0
	add.w	(g_CameraX).w,d0
	move.w	d0,$3E(a6)
	bsr.s	Boss_Claw_StepSwing
	lea.l	(g_Rot3DOuter).l,a0
	move.w	d0,$4(a0)
	move.w	#$FFE0,$8(a0)
	clr.w	(a0)
	lea.l	(g_Rot3DInner).l,a0
	clr.w	$4(a0)
	clr.w	$8(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_Claw_SwingFingers - claw slot-2 native: swings the fingers' frame about its second angle (g_Rot3DInner +4) with the claw itself fixed at tilt -$20, then places the fingers
; In:  a6 = the claw
; Out: -
; Evidence: installed by Op2_StartCoroutine2Rel at $1E8D0; TAS only
; ---------------------------------------------------------------------------
Boss_Claw_SwingFingers:
	bsr.w	Boss_Claw_StepSwing
	lea.l	(g_Rot3DInner).l,a0
	move.w	d0,$4(a0)
	clr.w	$8(a0)
	lea.l	(g_Rot3DOuter).l,a0
	move.w	#$FFE0,$8(a0)
	clr.w	$4(a0)
	clr.w	(a0)
	bra.w	Boss_Claw_PlaceFingers
; ---------------------------------------------------------------------------
; Boss_Claw_Draw - claw native (switch): builds the claw's 3D matrix, draws its 12-ball chain and its three 3-segment fingers, and returns whether the claw is at the players' depth (its attack box may hit)
; In:  a6 = the claw (+$E chain sprite block, +$10/+$12/+$14 finger blocks)
; Out: d0 = $2E(a6): 1 when the chain head's depth is $700-$800
; Evidence: Rot3D_BuildMatrix g_Rot3DOuter, Boss_Claw_DrawChain Tbl_ClawChainPoints (12 points x 10 bytes = its $78-byte span), Boss_Claw_DrawPoints x3; op $2D30 at $1E7B6 branches to Op2_TestAttackBoxAtDrawn, also op $2CB2 at $1E7C6; TAS f10512-f12925
; ---------------------------------------------------------------------------
Boss_Claw_Draw:
	movem.l	a2-a6,-(a7)
	lea.l	(g_Rot3DOuter).l,a0
	jsr	(Rot3D_BuildMatrix).l
	movea.w	$E(a6),a5
	lea.l	(Tbl_ClawChainPoints).l,a1
	moveq	#$B,d7
	bsr.w	Boss_Claw_DrawChain
	movea.w	$10(a6),a1
	moveq	#$2,d7
	bsr.w	Boss_Claw_DrawPoints
	movea.w	$12(a6),a1
	moveq	#$2,d7
	bsr.w	Boss_Claw_DrawPoints
	movea.w	$14(a6),a1
	moveq	#$2,d7
	bsr.w	Boss_Claw_DrawPoints
	movem.l	(a7)+,a2-a6
	move.w	$2E(a6),d0
	rts
