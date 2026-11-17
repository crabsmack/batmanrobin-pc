; ---------------------------------------------------------------------------
; Op4_BranchScriptTimerZero - a4 op: branches when the shared countdown g_ScriptTimer has run out (0); otherwise continues past the offset
; In:  a4 = script: word branch offset
; Out: NEXT or the branch (Op4_Goto)
; Evidence: tst.w g_ScriptTimer / beq Op4_Goto / addq #2; script use $3700C (`646E 000C`); 29 corpus runs
; ---------------------------------------------------------------------------
Op4_BranchScriptTimerZero:
	tst.w	(g_ScriptTimer).w
	beq.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_AddScriptTimer - a4 op: adds a word to the shared countdown g_ScriptTimer (Camera_UpdateView counts it down each frame)
; In:  a4 = script: word frames
; Out: NEXT
; Evidence: add.w d0,g_ScriptTimer; script use $3701A (`647C 0004`); 29 corpus runs
; ---------------------------------------------------------------------------
Op4_AddScriptTimer:
	move.w	(a4)+,d0
	add.w	d0,(g_ScriptTimer).w
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_PlaySound - a4 op: plays sound effect n through the sound driver request $1E5F88 (as Op2_PlaySound)
; In:  a4 = script: word sound number
; Out: NEXT
; Evidence: move.w (a4)+,d0 / jsr Snd_PlaySfx; 53 script uses, e.g. $13246 (`6486 0012`, sound $12 = the health pickup), $14C48 (`6486 0017`); 149 corpus runs
; ---------------------------------------------------------------------------
Op4_PlaySound:
	move.w	(a4)+,d0
	jsr	(Snd_PlaySfx).l
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_FlashRandom - a4 op: on a coin toss (Rand_Next negative) flashes the screen white for 4 frames (g_FlashReq, g_FlashTimer); otherwise does nothing
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: bsr Rand_Next / bpl VM4_Next / st g_FlashReq / move.b #4,g_FlashTimer; script use $20EC2; 3 corpus runs
; ---------------------------------------------------------------------------
Op4_FlashRandom:
	bsr.w	Rand_Next
	bpl.w	VM4_Next
	st.b	(g_FlashReq).w
	move.b	#$4,(g_FlashTimer).w
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ShakeRandom - a4 op: on a coin toss (Rand_Next negative) starts the standard screen shake (Shake_Start); otherwise does nothing
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: bsr Rand_Next / bpl VM4_Next / jsr Shake_Start; script use $20E98; 3 corpus runs
; ---------------------------------------------------------------------------
Op4_ShakeRandom:
	bsr.w	Rand_Next
	bpl.w	VM4_Next
; ---------------------------------------------------------------------------
; Op4_Shake - a4 op: starts the standard screen shake (Shake_Start)
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: dispatched entry inside Op4_ShakeRandom (computed-jump target in 7 corpus runs); script uses $1AAA2 (`64B0 63A2`), $1F124, $1F214
; ---------------------------------------------------------------------------
Op4_Shake:
	jsr	(Shake_Start).l
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_BranchCrossY - a4 op: line test on the drawn y: saves d4 to $56 and, when d5 reaches or crosses the line y (from either side, compared with the last y $58), snaps d5 and $58 to the line and branches; otherwise stores d5 in $58 and continues
; In:  a4 = script: word line y (world), word branch offset; d4/d5 = drawn x/y words
; Out: NEXT or the branch; $56/$58 = drawn x/y
; Evidence: cmp with $58(a6) picks the side, then cmp with d5; script uses $1A3A2 (`64BA 00C3 0004`), $1E36C (`64BA 00B4`, after Op4_Move); 4 corpus runs
; ---------------------------------------------------------------------------
Op4_BranchCrossY:
	move.w	d4,$56(a6)
	move.w	(a4)+,d0
	cmp.w	$58(a6),d0
	bgt.s	loc_0064D6
	cmp.w	d5,d0
	blt.s	loc_0064DA
loc_0064CA:
	move.w	d0,d5
	move.w	d5,$58(a6)
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
loc_0064D6:
	cmp.w	d5,d0
	ble.s	loc_0064CA
loc_0064DA:
	move.w	d5,$58(a6)
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_TerrainMove - a4 op: moves an a4 object's box against the scene terrain (g_TerrainFn) from its last position $56/$58 to the drawn d4/d5, keeps the result in d4/d5 and $56/$58, and branches on any wall or floor contact
; In:  a4 = script: word wall mask d6, word floor mask d7, words box dx, dy (added for the test), word branch offset; d4/d5 = drawn x/y words
; Out: NEXT or the branch; d4/d5 = corrected position
; Evidence: d1/d2 = d4/d5 - $56/$58, jsr (g_TerrainFn), or.w d6,d7 / bne Op4_Goto (the Coll_ contract); script uses $1F102 (`64E4 0001 0001 0000 0008 0018`), $26AFA; 26 corpus runs
; ---------------------------------------------------------------------------
Op4_TerrainMove:
	move.w	d4,d1
	move.w	d5,d2
	move.w	(a4)+,d6
	move.w	(a4)+,d7
	movem.w	$56(a6),d4-d5
	sub.w	d4,d1
	sub.w	d5,d2
	add.w	(a4),d4
	add.w	$2(a4),d5
	movea.l	(g_TerrainFn).w,a0
	jsr	(a0)
	sub.w	(a4)+,d4
	sub.w	(a4)+,d5
	movem.w	d4-d5,$56(a6)
	or.w	d6,d7
	bne.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_TerrainMoveDefault - a4 op: Op4_TerrainMove with masks 9/9 and no box offset: moves $56/$58 to d4/d5 against g_TerrainFn and branches on contact
; In:  a4 = script: word branch offset; d4/d5 = drawn x/y words
; Out: NEXT or the branch; d4/d5 = corrected position
; Evidence: moveq #9,d6 / moveq #9,d7 / jsr (g_TerrainFn); script uses $1A9B4 (`6518 0004 677E 63E2`: on contact Op4_BounceYRandom), $358B2; 5 corpus runs
; ---------------------------------------------------------------------------
Op4_TerrainMoveDefault:
	moveq	#$9,d6
	moveq	#$9,d7
	move.w	d4,d1
	move.w	d5,d2
	movem.w	$56(a6),d4-d5
	sub.w	d4,d1
	sub.w	d5,d2
	movea.l	(g_TerrainFn).w,a0
	jsr	(a0)
	movem.w	d4-d5,$56(a6)
	or.w	d6,d7
	bne.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
