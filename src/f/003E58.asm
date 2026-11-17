; ---------------------------------------------------------------------------
; Op2_RunAnim - a2 op: steps the current sprite block's animation script: counts the frame timer $1E(a5) down by the object's speed $2E(a4) and, when it runs out, runs the animation script at $2E(a5) (ops chained by NEXT until one returns with rts) and saves where it stopped
; In:  a2 = script (no operand); a4 = object block, a5 = sprite block (+$2E animation pc, +$1E timer)
; Out: NEXT in the calling script
; Evidence: sub.w d0,$1E(a5) / bgt $7E0; push a2, a2 = $2E(a5), jsr (a0), move.l a2,$2E(a5) at the return; Op2_SetAnim/SetBlockAnim start these scripts; every object loop (127k TAS frames, all 160 runs), e.g. $13310 `3E58 448A`
; ---------------------------------------------------------------------------
Op2_RunAnim:
	move.w	$2E(a4),d0
	sub.w	d0,$1E(a5)
	bgt.w	VM2_Next
	move.l	a2,-(a7)
	movea.l	$2E(a5),a2
	movea.w	(a2)+,a0
	jsr	(a0)
	move.l	a2,$2E(a5)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimGoto - animation-script op: relative jump that keeps running animation ops (same code as Op2_Goto, used to loop animation scripts)
; In:  a2 = animation script: word offset
; Out: NEXT at the target
; Evidence: adda.w (a2),a2; 152 corpus runs; $1B3BA anim script ends `3E78 FFEA` back to its first frame
; ---------------------------------------------------------------------------
Op2_AnimGoto:
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimStop - animation-script op: ends the animation: stays on this op for good and sets the done flag (bit 0 of $18(a5)) that Op2_BranchAnimFlag tests
; In:  a2 = animation script (no operand)
; Out: a2 = this op again, rts to Op2_RunAnim
; Evidence: subq.l #2,a2 / bset #0,$18(a5) / rts; 152 corpus runs, 195 script uses (e.g. $19AB0 `3E7E`)
; ---------------------------------------------------------------------------
Op2_AnimStop:
	subq.l	#$2,a2
	bset	#$0,$18(a5)
	rts
; ---------------------------------------------------------------------------
; Op2_SetAnimLoop - animation-script op: sets the animation loop counter $20(a5)
; In:  a2 = animation script: word count
; Out: NEXT
; Evidence: move.w (a2)+,$20(a5); counted down by Op2_AnimLoop; 34 corpus runs; $27430 `3E88 0007`
; ---------------------------------------------------------------------------
Op2_SetAnimLoop:
	move.w	(a2)+,$20(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetAnimLoopRand - animation-script op: sets the animation loop counter $20(a5) to base + Rand_Below(range)
; In:  a2 = animation script: word base, word range
; Out: NEXT
; Evidence: move.w (a2)+,d1 / move.w (a2)+,d0 / bsr Rand_Below / add.w d1,d0 / move.w d0,$20(a5); 5 corpus runs; $1BC98 `3E90 0001 0001`
; ---------------------------------------------------------------------------
Op2_SetAnimLoopRand:
	move.w	(a2)+,d1
	move.w	(a2)+,d0
	bsr.w	Rand_Below
	add.w	d1,d0
	move.w	d0,$20(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimLoop - animation-script op: decrements the loop counter $20(a5) and jumps back while it stays above 0
; In:  a2 = animation script: word offset
; Out: branch while $20(a5) > 0, else NEXT after the offset
; Evidence: subq.w #1,$20(a5) / bgt Op2_Goto; 40 corpus runs; $1BCB2 `3EA2 FFEA`
; ---------------------------------------------------------------------------
Op2_AnimLoop:
	subq.w	#$1,$20(a5)
	bgt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimGotoYield - animation-script op: jumps to a relative target and ends this step with the timer cleared, so the animation resumes there on the next run
; In:  a2 = animation script: word offset
; Out: a2 = target, $1E(a5) = 0, rts to Op2_RunAnim
; Evidence: adda.w (a2),a2 then the Op2_AnimYield tail; 3 corpus runs; $19D58 `3EB0 FFEA`
; ---------------------------------------------------------------------------
Op2_AnimGotoYield:
	adda.w	(a2),a2
; ---------------------------------------------------------------------------
; Op2_AnimYield - animation-script op: ends this animation step at once with the timer cleared (resumes at the next op on the next run)
; In:  a2 = animation script (no operand)
; Out: $1E(a5) = 0, rts to Op2_RunAnim
; Evidence: dispatched at $3EB2 (clr.w $1E(a5) / rts; 14 corpus runs); $250A0 `3EB2 3EB8 0449`
; ---------------------------------------------------------------------------
Op2_AnimYield:
	clr.w	$1E(a5)
	rts
; ---------------------------------------------------------------------------
; Op2_SetAnimDelay - animation-script op: sets the per-frame display time $1C(a5) (in 1/256 timer units: $100 = one run at speed $100) and clears the timer
; In:  a2 = animation script: word delay
; Out: NEXT
; Evidence: move.w (a2)+,$1C(a5) / clr.w $1E(a5); Op2_AnimFrame adds $1C to the timer that Op2_RunAnim counts down by $2E(a4) ($100 = speed 1.0); 152 runs; $1B3BA `3EB8 0A00`
; ---------------------------------------------------------------------------
Op2_SetAnimDelay:
	move.w	(a2)+,$1C(a5)
	clr.w	$1E(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetAnimDelayRand - animation-script op: sets the frame display time $1C(a5) to base + Rand_Signed(spread) and clears the timer
; In:  a2 = animation script: word spread, word base
; Out: NEXT
; Evidence: move.w (a2)+,d0 / bsr Rand_Signed / add.w (a2)+,d0 / move.w d0,$1C(a5); 14 corpus runs; $2624A `3EC4 0300 0400`
; ---------------------------------------------------------------------------
Op2_SetAnimDelayRand:
	move.w	(a2)+,d0
	bsr.w	Rand_Signed
	add.w	(a2)+,d0
	move.w	d0,$1C(a5)
	clr.w	$1E(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AddAnimDelay - animation-script op: adds a signed word to the frame display time $1C(a5) (speeds up or slows down the animation)
; In:  a2 = animation script: word delta
; Out: NEXT
; Evidence: add.w d0,$1C(a5); 50 corpus runs; $28524 `3ED8 FE00 3EA2 FFE6` (shorter each loop)
; ---------------------------------------------------------------------------
Op2_AddAnimDelay:
	move.w	(a2)+,d0
	add.w	d0,$1C(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimHoldByVelY - animation-script op: every run, picks a case by the vertical speed band ((vy + ay + bias $32(a5)) / $36(a5)), runs that case's single op (e.g. a frame) and stays on this op; falls through to the next op once the speed is above every threshold
; In:  a2 = animation script: word n, then n pairs (word threshold, word offset); a3 +$1C/+$28 = y velocity/acceleration
; Out: case: one op run, timer cleared, a2 = this op, rts; none: NEXT after the table
; Evidence: pea -2(a2) / $1C(a3)+$28(a3)+$32(a5), divu $36(a5) / cmp.w (a2)+,d0 / ble -> jsr (a0) / clr.w $1E(a5) / movea.l (a7)+,a2 / rts (negative -> first case); 89 corpus runs; $1B4A6 `3EE2 0001 0028 000E`
; ---------------------------------------------------------------------------
Op2_AnimHoldByVelY:
	pea.l	-$2(a2)
	move.l	$1C(a3),d0
	add.l	$28(a3),d0
	add.l	$32(a5),d0
	bmi.s	loc_003F0C
	divu.w	$36(a5),d0
	move.w	(a2)+,d1
	bra.s	loc_003F02
loc_003EFC:
	cmp.w	(a2)+,d0
	ble.s	loc_003F0E
	addq.l	#$2,a2
loc_003F02:
	dbra	d1,loc_003EFC
	addq.l	#$4,a7
	movea.w	(a2)+,a0
	jmp	(a0)
loc_003F0C:
	addq.l	#$4,a2
loc_003F0E:
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jsr	(a0)
	clr.w	$1E(a5)
	movea.l	(a7)+,a2
	rts
; ---------------------------------------------------------------------------
; Op2_SwitchVelXBand - a2 op: branches by the horizontal speed band: (vx + bias $32(a5)) / $36(a5) against ascending thresholds; the last case is the default
; In:  a2 = script: word n, then n pairs (word threshold, word offset), the last threshold unused
; Out: NEXT at the first case whose threshold >= the value
; Evidence: move.l $18(a3),d0 / add.l $32(a5) / divs.w $36(a5) / cmp.w (a2)+,d0 / ble; 25 corpus runs; $2819C `3F1C 0004 000A 0058 001E 0088 ...`
; ---------------------------------------------------------------------------
Op2_SwitchVelXBand:
	move.l	$18(a3),d0
	bra.s	loc_003F26
; ---------------------------------------------------------------------------
; Op2_SwitchVelYBand - a2 op: as Op2_SwitchVelXBand on the vertical velocity $1C(a3)
; In:  a2 = script: word n, then n pairs (word threshold, word offset), the last threshold unused
; Out: NEXT at the chosen case
; Evidence: move.l $1C(a3),d0 then the shared band search $3F26; 54 corpus runs; $28078 `3F22 0005 FFC4 004A ...`
; ---------------------------------------------------------------------------
Op2_SwitchVelYBand:
	move.l	$1C(a3),d0
loc_003F26:
	add.l	$32(a5),d0
	divs.w	$36(a5),d0
	move.w	(a2)+,d1
	subq.w	#$2,d1
	bmi.s	loc_003F3E
loc_003F34:
	cmp.w	(a2)+,d0
	ble.s	loc_003F40
	addq.l	#$2,a2
	dbra	d1,loc_003F34
loc_003F3E:
	addq.l	#$2,a2
loc_003F40:
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
