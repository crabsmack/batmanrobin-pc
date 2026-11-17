; ---------------------------------------------------------------------------
; Op4_SetTimer - a4 op: sets the object's timer word $16(a6)
; In:  a4 = script: word frames
; Out: NEXT
; Evidence: move.w (a4)+,$16(a6); counted by Op4_TimerBranchOut / Op4_TimerLoop; 24 a4-script uses ($1504E); 54 corpus runs
; ---------------------------------------------------------------------------
Op4_SetTimer:
	move.w	(a4)+,$16(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetTimerRandom - a4 op: sets the object's timer $16(a6) to base + a random number below range
; In:  a4 = script: word range, word base
; Out: NEXT
; Evidence: Rand_Below(range) + base into $16(a6); script use $46AC8; 4 corpus runs
; ---------------------------------------------------------------------------
Op4_SetTimerRandom:
	move.w	(a4)+,d0
	bsr.w	Rand_Below
	add.w	(a4)+,d0
	move.w	d0,$16(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetTimer - a4 op: sets the timer word $16 of the last spawned child (a5)
; In:  a4 = script: word frames; a5 = child (a6 when none was spawned this run)
; Out: NEXT
; Evidence: move.w (a4)+,$16(a5); a5 = the new object after the spawn ops (Op4_Spawn); script uses $25B7A, $25B98; 44 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetTimer:
	move.w	(a4)+,$16(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetTimerRandom - a4 op: sets the child's (a5) timer $16 to base + a random number below range
; In:  a4 = script: word range, word base
; Out: NEXT
; Evidence: Rand_Below(range) + base into $16(a5); script uses $36F9E, $36FC0; 29 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetTimerRandom:
	move.w	(a4)+,d0
	bsr.w	Rand_Below
	add.w	(a4)+,d0
	move.w	d0,$16(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_TimerBranchOut - a4 op: counts the timer $16(a6) down by one and branches when it runs out (<= 0); otherwise continues past the offset
; In:  a4 = script: word offset
; Out: Op4_Goto when expired, else NEXT after the offset
; Evidence: subq.w #1,$16(a6) / ble Op4_Goto; script uses $1514C, $26B8E; 89 corpus runs
; ---------------------------------------------------------------------------
Op4_TimerBranchOut:
	subq.w	#$1,$16(a6)
	ble.s	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_TimerLoop - a4 op: counts the timer $16(a6) down by one and branches (loops back) while it has not run out; once <= 0 continues past the offset
; In:  a4 = script: word offset
; Out: branch while > 0, else NEXT after the offset (Op4_SkipWord)
; Evidence: subq.w #1,$16(a6) / ble Op4_SkipWord / adda.w (a4),a4; script uses $1505A, $15098 (negative offsets to the motion/draw loop); 48 corpus runs
; ---------------------------------------------------------------------------
Op4_TimerLoop:
	subq.w	#$1,$16(a6)
	ble.w	Op4_SkipWord
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetCounterTicker - a4 op: sets the counter byte $19(a6) (low byte of the first word) and the ticker word $1A(a6) (accumulator byte $1A, rate byte $1B)
; In:  a4 = script: word count (low byte used), word accumulator:rate
; Out: NEXT
; Evidence: move.b d0,$19(a6) / move.w (a4)+,$1A(a6); counter read by Op4_CounterKillOut/BranchOut, ticker by Op4_WaitTicker/TickerLoop; script uses $14E40, $1A3FA; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_SetCounterTicker:
	move.w	(a4)+,d0
	move.b	d0,$19(a6)
	move.w	(a4)+,$1A(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SetTicker - a4 op: sets the ticker word $1A(a6): accumulator byte $1A and per-frame rate byte $1B (the ticker fires when the byte sum carries, rate/256 per frame)
; In:  a4 = script: word accumulator:rate
; Out: NEXT
; Evidence: move.w (a4)+,$1A(a6); e.g. $26D52 sets $FF40 then Op4_TickerLoop; 71 corpus runs
; ---------------------------------------------------------------------------
Op4_SetTicker:
	move.w	(a4)+,$1A(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_CounterKillOut - a4 op: counts the counter byte $19(a6) down and deletes the object when it runs out (0 or below); otherwise continues
; In:  a4 = script (no operand)
; Out: Op4_Kill when out, else NEXT
; Evidence: subq.b #1,$19(a6) / bls Op4_Kill; script uses $14E52, $1A412, $1A5C2 (before the motion op $6286): an effect's life in frames; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_CounterKillOut:
	subq.b	#$1,$19(a6)
	bls.w	Op4_Kill
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_CounterBranchOut - a4 op: counts the counter byte $19(a6) down and branches when it runs out; otherwise continues past the offset
; In:  a4 = script: word offset
; Out: Op4_Goto when out, else NEXT after the offset
; Evidence: subq.b #1,$19(a6) / bls Op4_Goto; script use $359CA; 3 corpus runs
; ---------------------------------------------------------------------------
Op4_CounterBranchOut:
	subq.b	#$1,$19(a6)
	bls.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_WaitTicker - a4 op: waits on the ticker: adds the rate $1B(a6) to the accumulator $1A(a6) and continues on a carry; otherwise ends the frame and retries this op next frame
; In:  a4 = script (no operand)
; Out: NEXT on a carry; else $8(a6) = this op, rts
; Evidence: add.b $1B,$1A / bcs VM4_Next / subq.l #2,a4 / move.l a4,$8(a6) / rts; script uses $14E46, $1A400, $1E262; 9 corpus runs
; ---------------------------------------------------------------------------
Op4_WaitTicker:
	move.b	$1B(a6),d0
	add.b	d0,$1A(a6)
	bcs.w	VM4_Next
	subq.l	#$2,a4
	move.l	a4,$8(a6)
	rts
; ---------------------------------------------------------------------------
; Op4_TickerLoop - a4 op: steps the ticker ($1A(a6) += rate $1B) and branches (loops back) until it carries; on the carry continues past the offset
; In:  a4 = script: word offset
; Out: Op4_Goto while no carry, else NEXT after the offset
; Evidence: add.b $1B,$1A / bcc Op4_Goto; script uses $1A5B0, $1AA86, $1F220 (after Op4_SetTicker); 69 corpus runs
; ---------------------------------------------------------------------------
Op4_TickerLoop:
	move.b	$1B(a6),d0
	add.b	d0,$1A(a6)
	bcc.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
