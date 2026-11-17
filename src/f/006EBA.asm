; ---------------------------------------------------------------------------
; Op5_WaitPopulation6 - a5 op: waits until g_ObjPopulation is at most 6
; In:  a5 = script (no operands)
; Out: NEXT, or yields with the pc on this op
; Evidence: moveq #6 then the Op5_WaitPopulation0 test; script use $34550 (`6EBA 6D10`); 3 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation6:
	moveq	#$6,d0
	bra.s	loc_006ED4
; ---------------------------------------------------------------------------
; Op5_WaitPopulation5 - a5 op: waits until g_ObjPopulation is at most 5
; In:  a5 = script (no operands)
; Out: NEXT, or yields
; Evidence: moveq #5; script uses $34A24, $34A40; 1 corpus run
; ---------------------------------------------------------------------------
Op5_WaitPopulation5:
	moveq	#$5,d0
	bra.s	loc_006ED4
; ---------------------------------------------------------------------------
; Op5_WaitPopulation4 - a5 op: waits until g_ObjPopulation is at most 4
; In:  a5 = script (no operands)
; Out: NEXT, or yields
; Evidence: moveq #4; script uses $34226, $3432A; 7 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation4:
	moveq	#$4,d0
	bra.s	loc_006ED4
; ---------------------------------------------------------------------------
; Op5_WaitPopulation3 - a5 op: waits until g_ObjPopulation is at most 3
; In:  a5 = script (no operands)
; Out: NEXT, or yields
; Evidence: moveq #3; script uses $41A26, $41A74; 3 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation3:
	moveq	#$3,d0
	bra.s	loc_006ED4
; ---------------------------------------------------------------------------
; Op5_WaitPopulation2 - a5 op: waits until g_ObjPopulation is at most 2
; In:  a5 = script (no operands)
; Out: NEXT, or yields
; Evidence: moveq #2; script uses $16E44, $3423E; 3 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation2:
	moveq	#$2,d0
	bra.s	loc_006ED4
; ---------------------------------------------------------------------------
; Op5_WaitPopulation1 - a5 op: waits until g_ObjPopulation is at most 1
; In:  a5 = script (no operands)
; Out: NEXT, or yields
; Evidence: moveq #1; 39 script uses, e.g. $16B42 (`6ECE 6AC4`), $16B64; 13 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation1:
	moveq	#$1,d0
	bra.s	loc_006ED4
; ---------------------------------------------------------------------------
; Op5_WaitPopulation0 - a5 op: waits until g_ObjPopulation is 0 (every counted enemy gone); the shared test and wait tail of Op5_WaitPopulation*
; In:  a5 = script (no operands)
; Out: NEXT, or yields with timer 0, the pc on this op
; Evidence: cmp.w g_ObjPopulation / bge VM5_Next / subq.l #2,a5; clr.l $2E, $1E = a5, restore g_VM5SavedSP, rts; 259 script uses, e.g. $16B86, $16B8C (`6ED2 6AC4 03C0 6E9A 6A72`); 86 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation0:
	moveq	#$0,d0
loc_006ED4:
	cmp.w	(g_ObjPopulation).w,d0
	bge.w	VM5_Next
	subq.l	#$2,a5
; ---------------------------------------------------------------------------
; Op5_WaitYield - shared wait tail of the a5 population waits: the timer cleared (re-checked next frame), the pc left on the waiting op, then the yield
; In:  a5 = the op to resume at, a6 = wave object
; Out: $2E = 0, $1E = a5; sp = g_VM5SavedSP, rts to VM5_RunObject's caller
; Evidence: clr.l $2E(a6) / move.l a5,$1E(a6) / movea.l (g_VM5SavedSP).w,a7 / rts; bra from Op5_WaitPopulation (after subq.l #4,a5), the fall-in of Op5_WaitPopulation0-6 (subq.l #2,a5); 86 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitYield:
	clr.l	$2E(a6)
	move.l	a5,$1E(a6)
	movea.l	(g_VM5SavedSP).w,a7
	rts
; ---------------------------------------------------------------------------
; Op5_EndOtherWaves - a5 op: ends every other wave director: takes a fresh serial (++g_WaveSerial) for both g_WaveGen and this wave's $A, so all others (serial below g_WaveGen) end on their next frame
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: g_WaveSerial+1 -> g_WaveGen and $A(a6); VM5_RunObject ends a wave when g_WaveGen > $A; script uses $2201E, $22042; 3 corpus runs
; ---------------------------------------------------------------------------
Op5_EndOtherWaves:
	addq.w	#$1,(g_WaveSerial).w
	move.w	(g_WaveSerial).w,d0
	move.w	d0,(g_WaveGen).w
	move.w	d0,$A(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_EndOlderWaves - a5 op: ends the wave directors started before this one: g_WaveGen = this wave's serial $A (newer waves keep running)
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: move.w $A(a6),g_WaveGen; 244 script uses, usually the first op, e.g. $1693A (`6F00 6DD4 0003 0004 6FB2`), $21DAC; 40 corpus runs
; ---------------------------------------------------------------------------
Op5_EndOlderWaves:
	move.w	$A(a6),(g_WaveGen).w
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_StartGroup - a5 op: starts a kill group for the following spawns: a pool block (+0 members, +2 kills left, +4 reward spawn action, +$A deadline frame or 0, +6 spawned, +8 dead) linked from $32(a6); an older group still alive is closed at its spawned count first
; In:  a5 = script: word members, word reward action (Tbl_012EF0), word time limit in frames (0 = none)
; Out: NEXT; on a full pool $32 = 0 and only 4 operand bytes are skipped (the third word would then run as an op: inferred, never seen)
; Evidence: ObjPool_Alloc, deadline = g_FrameCounter + limit; members get spawn action 1 from the spawn routine $6FEA (SpawnAction_GroupMemberReward); script uses $16B66 (`6F0A 0005 001B 00B4`: kill 5 within 180 frames -> action $1B), $17012; 27 corpus runs
; ---------------------------------------------------------------------------
Op5_StartGroup:
	move.w	$32(a6),d0
	beq.s	loc_006F1E
	movea.w	d0,a0
	move.w	$6(a0),d0
	cmp.w	$8(a0),d0
	ble.s	loc_006F28
	move.w	d0,(a0)
loc_006F1E:
	bsr.w	ObjPool_Alloc
	bne.s	loc_006F4C
	move.w	a0,$32(a6)
loc_006F28:
	move.w	(a5),(a0)
	move.w	(a5)+,$2(a0)
	move.w	(a5)+,$4(a0)
	moveq	#$0,d0
	move.w	(a5)+,d0
	beq.s	loc_006F3C
	add.l	(g_FrameCounter).w,d0
loc_006F3C:
	move.l	d0,$A(a0)
	clr.w	$6(a0)
	clr.w	$8(a0)
	movea.w	(a5)+,a0
	jmp	(a0)
loc_006F4C:
	clr.w	$32(a6)
	addq.l	#$4,a5
	movea.w	(a5)+,a0
	jmp	(a0)
