; ---------------------------------------------------------------------------
; Stage3_PlaceRoadEndSwirl - puts the road-end swirl at the far end of the road: x = ($C4 + the last row's x offset) x its scale / 4096 + the road's screen origin g_SceneVars2+$3A, y = the last row's line + g_SceneVars2+$3C + 1
; In:  a6 = the swirl; the last road row entry at g_SceneVars+$A2 (word line, word -, word x offset, word scale) written by Stage3_DrawRoad
; Out: $3E/$42(a6), d0 = 0 (Op2_CallNativeRel)
; Evidence: Op2_CallNativeRel at $03DBD2 in Scr2_03DBC6's frame loop; muls/asl #4/swap; TAS f113312-f116145 (2834 frames)
; ---------------------------------------------------------------------------
Stage3_PlaceRoadEndSwirl:
	lea.l	(g_SceneVars+$A2).l,a1
	move.w	(a1)+,d5
	addq.l	#$2,a1
	move.w	#$C4,d4
	add.w	(a1)+,d4
	muls.w	(a1),d4
	asl.l	#$4,d4
	swap	d4
	add.w	(g_SceneVars2+$3A).w,d4
	add.w	(g_SceneVars2+$3C).w,d5
	addq.w	#$1,d5
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_StepRoadSchedule - per-frame step of the Hatter's road schedule: counts the road's travel in g_SceneVars+2 (minus the speed g_SceneVars2+$C, mod $80); each time a checker row ($80) passes it counts down the row wait g_SceneVars+$10 and, when that runs out, runs 2-byte commands from the schedule pointer in the boss's coroutine slot 2 (a4 +$40) through Boss_Hatter_RoadCmdTbl until one waits: low nibble 0 end, 1 wait byte1 rows, 2 goto (12-bit offset), 3 gosub (stack g_SceneVars+8, depth +4), 4 return, 5 set loop count (+6), 6 loop, 7 random pick from a word-offset table, 8-15 spawn kind 0-7 in lane = high nibble then wait byte1 rows
; In:  a4 = the boss's a4 block (+$40 = schedule pointer or 0)
; Out: g_SceneVars+2/+4/+6/+8../+$10, a4 +$40 (0 after the end command)
; Evidence: Op2_CallNativeRelSaved $03DC94 at $03D0C4/$03D12C in the boss's frame loops; `jmp (pc,d0.w)` into 16 bra.s at $03DD32; schedule Tbl_03DDD0 = 0000 (end), $03F5BE = 1805 2805 .. 7805 .. 2805 F2E8 (a zigzag of lanes 1-7, 5 rows apart, goto back); TAS f113312-f116145
; ---------------------------------------------------------------------------
Boss_Hatter_StepRoadSchedule:
	move.w	(g_SceneVars+$2).w,d0
	sub.w	(g_SceneVars2+$C).w,d0
	bcs.s	Boss_Hatter_RoadSchedRowPassed
	andi.w	#$7F,d0
	move.w	d0,(g_SceneVars+$2).w
; ---------------------------------------------------------------------------
; Boss_Hatter_RoadSchedIdle - the road schedule step's return: no checker row passed, no schedule, or the row wait still running
; In:  -
; Out: -
; Evidence: rts at $03DCA6; bcs-not-taken path of Boss_Hatter_StepRoadSchedule, beq from the slot-2 test, bpl from the wait countdown; a function of the generated code (P4 entries)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadSchedIdle:
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_RoadSchedRowPassed - a checker row has passed: wraps the travel count to the row, then (with a schedule in the boss's slot 2) counts down the row wait
; In:  d0 = travel - speed (borrowed), a4 = the boss's a4 block
; Out: g_SceneVars+2; a0 = the schedule pointer
; Evidence: bcs target in Boss_Hatter_StepRoadSchedule ($03DCA8): andi.w #$7F / move.l $40(a4),d0 / beq / movea.l d0,a0; TAS f113312-f116145
; ---------------------------------------------------------------------------
Boss_Hatter_RoadSchedRowPassed:
	andi.w	#$7F,d0
	move.w	d0,(g_SceneVars+$2).w
	move.l	$40(a4),d0
	beq.s	Boss_Hatter_RoadSchedIdle
	movea.l	d0,a0
; ---------------------------------------------------------------------------
; Boss_Hatter_RoadSchedCountWait - counts down the schedule's row wait g_SceneVars+$10 and, when it runs out (below 0), reads the next command
; In:  a0 = the schedule pointer, a4 = the boss's a4 block
; Out: g_SceneVars+$10; as Boss_Hatter_RoadCmdNext
; Evidence: subq.b #1,(g_SceneVars+$10).w / bpl rts; bra.w from Boss_Hatter_RoadCmdSpawnInLane after its spawner; TAS f114753-f114833
; ---------------------------------------------------------------------------
Boss_Hatter_RoadSchedCountWait:
	subq.b	#$1,(g_SceneVars+$10).w
	bpl.s	Boss_Hatter_RoadSchedIdle
; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdNext - reads the road schedule's next 2-byte command and jumps to its handler in Boss_Hatter_RoadCmdTbl by the low nibble of byte 0
; In:  a0 = the command, a4 = the boss's a4 block
; Out: a0 past the command, d4 = byte 0, d1 = byte 1, d0 = nibble x 2
; Evidence: move.b (a0)+,d0 / move.b (a0)+,d1 / move.b d0,d4 / andi.w #$F / add.w / jmp Boss_Hatter_RoadCmdTbl(pc,d0.w); bra.s from Boss_Hatter_RoadCmdGoto and the raw commands 5, 6, 4, 7
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdNext:
	move.b	(a0)+,d0
	move.b	(a0)+,d1
	move.b	d0,d4
	andi.w	#$F,d0
	add.w	d0,d0
	jmp	Boss_Hatter_RoadCmdTbl(pc,d0.w)
; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdEnd - road schedule command 0: ends the schedule (clears the gosub depth, the row wait and the boss's slot 2)
; In:  a4 = the boss's a4 block
; Out: g_SceneVars+4, +$10, a4 +$40 = 0
; Evidence: clr.w g_SceneVars+4 / clr.w g_SceneVars+$10 / clr.l $40(a4) / rts; bra.s from Boss_Hatter_RoadCmdTbl entry 0; ctarget in 3 corpus runs
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdEnd:
	clr.w	(g_SceneVars+$4).w
	clr.w	(g_SceneVars+$10).w
	clr.l	$40(a4)
	rts
