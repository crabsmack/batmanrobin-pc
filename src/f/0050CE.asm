; ---------------------------------------------------------------------------
; Op2_StopOscY - a2 op: stops the y axis of the oscillator block in an a6 field (max speed +$20 = 0)
; In:  a2 = script: word field offset
; Out: NEXT
; Evidence: clr.l $20(a0); 3 corpus runs; $3BA88 `50CE 000E`
; ---------------------------------------------------------------------------
Op2_StopOscY:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d0
	beq.w	VM2_Next
	movea.w	d0,a0
	clr.l	$20(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StepOsc - a2 op: steps the oscillator block in an a6 field and makes its velocities this frame's move d1/d2: each axis accelerates toward its signed max speed, moves, and flips the sign of limit, max speed and acceleration when the position passes the limit (a swing between -limit and +limit)
; In:  a2 = script: word field offset; block axes at +0 and +$14: position, limit, velocity, max speed, acceleration (longs)
; Out: d1/d2 = the two velocities (0 for an empty field); NEXT
; Evidence: moveq #0,d1/d2 then $50E6; movem.l (a0),d0/d4-d7 / add.l d7,d5 / clamp to d6 / add.l d5,d0 / cmp.l d4,d0 / neg.l d4,d6,d7; 27 corpus runs; $3B432 `50E2 0008 4C52`
; ---------------------------------------------------------------------------
Op2_StepOsc:
	moveq	#$0,d1
	moveq	#$0,d2
; ---------------------------------------------------------------------------
; Op2_AddOsc - a2 op: as Op2_StepOsc but adds the oscillator velocities to the move d1/d2 already computed (by Op2_StepVelocity), so the swing rides on the normal motion
; In:  a2 = script: word field offset; d1/d2 = move so far
; Out: d1/d2 += oscillator velocities; NEXT
; Evidence: dispatched at $50E6, after the two moveq of $50E2 (92 corpus runs); $133C0 `4B74 50E6 000E 4CDA`
; ---------------------------------------------------------------------------
Op2_AddOsc:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d0
	beq.w	VM2_Next
	movea.w	d0,a0
	movem.l	(a0),d0/d4-d7
	tst.l	d6
	bpl.s	loc_00510A
	add.l	d7,d5
	cmp.l	d6,d5
	bge.s	loc_005102
	move.l	d6,d5
loc_005102:
	add.l	d5,d0
	cmp.l	d4,d0
	bgt.s	loc_00511E
	bra.s	loc_005118
loc_00510A:
	add.l	d7,d5
	cmp.l	d6,d5
	ble.s	loc_005112
	move.l	d6,d5
loc_005112:
	add.l	d5,d0
	cmp.l	d4,d0
	blt.s	loc_00511E
loc_005118:
	neg.l	d4
	neg.l	d6
	neg.l	d7
loc_00511E:
	add.l	d5,d1
	movem.l	d0/d4-d7,(a0)
	movem.l	$14(a0),d0/d4-d7
	tst.l	d6
	bpl.s	loc_00513E
	add.l	d7,d5
	cmp.l	d6,d5
	bge.s	loc_005136
	move.l	d6,d5
loc_005136:
	add.l	d5,d0
	cmp.l	d4,d0
	bgt.s	loc_005152
	bra.s	loc_00514C
loc_00513E:
	add.l	d7,d5
	cmp.l	d6,d5
	ble.s	loc_005146
	move.l	d6,d5
loc_005146:
	add.l	d5,d0
	cmp.l	d4,d0
	blt.s	loc_005152
loc_00514C:
	neg.l	d4
	neg.l	d6
	neg.l	d7
loc_005152:
	add.l	d5,d2
	movem.l	d0/d4-d7,$14(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxSpawnRegsImpulse34 - a2 op: runs an a4 effect spawn script on g_SpawnCtx with the position/radius/heading registers left by the Op2_FxPos*/Op2_FxHeading* ops and 3/4 of the motion block's impulse $2/$6(a3) as the spawn velocity
; In:  a2 = script: word offset to the a4 script; d2 radius, d3 heading (high word), d4/d5 x/y words
; Out: effects spawned; NEXT
; Evidence: movem.l $2(a3),d6-d7 / asr.l #2 / sub.l (x3/4) then the shared spawn tail $519E; 20 corpus runs; $2065C `515E 0012`
; ---------------------------------------------------------------------------
Op2_FxSpawnRegsImpulse34:
	movem.l	$2(a3),d6-d7
	move.l	d6,d0
	asr.l	#$2,d0
	sub.l	d0,d6
	move.l	d7,d0
	asr.l	#$2,d0
	sub.l	d0,d7
	bra.s	loc_00519E
; ---------------------------------------------------------------------------
; Op2_FxSpawnRegsImpulse - a2 op: as Op2_FxSpawnRegs but the spawn velocity is the motion block's impulse $2/$6(a3)
; In:  a2 = script: word offset to the a4 script; d2-d5 from the Op2_FxPos*/Heading* ops
; Out: NEXT
; Evidence: movem.l $2(a3),d6-d7 / bra $519E; 30 corpus runs; $1AFE8 `5264 5172 05E4`
; ---------------------------------------------------------------------------
Op2_FxSpawnRegsImpulse:
	movem.l	$2(a3),d6-d7
	bra.s	loc_00519E
; ---------------------------------------------------------------------------
; Op2_FxSpawnOff - a2 op: runs an a4 effect spawn script on g_SpawnCtx at the object's position plus a signed byte offset (not mirrored), radius/heading 0, inheriting the object's velocity $18/$1C(a3)
; In:  a2 = script: byte dx, byte dy, word offset to the a4 script
; Out: NEXT
; Evidence: move.b (a2)+,d4/d5 / ext / add.w $3E/$42(a6) then $5194; 42 corpus runs, 62 script uses (e.g. $19666 `517A 0039 156C`)
; ---------------------------------------------------------------------------
Op2_FxSpawnOff:
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	bra.s	loc_005194
; ---------------------------------------------------------------------------
; Op2_FxSpawnHere - a2 op: runs an a4 effect spawn script on g_SpawnCtx at the object's position, radius/heading 0, inheriting the object's velocity: the context gets velocity $3E/$42, attr $22 and depth $24 from a5, the script's first op is called with d2-d5
; In:  a2 = script: word offset (from the operand) to the a4 script; a3/a5 = motion/sprite blocks
; Out: effects spawned; a1-a6 preserved; NEXT
; Evidence: lea g_SpawnCtx,a6 / movem.l d6-d7,$3E(a6) / clr.l $36,$3A / lea -2(a2,d0.w),a4 / swap d4,d5 / jsr (a0); same contract as Fx_RunScript; 64 corpus runs; $1998E `518C 10C6`
; ---------------------------------------------------------------------------
Op2_FxSpawnHere:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
loc_005194:
	moveq	#$0,d2
	moveq	#$0,d3
; ---------------------------------------------------------------------------
; Op2_FxSpawnRegs - a2 op: runs an a4 effect spawn script on g_SpawnCtx with the position/radius/heading registers prepared by the Op2_FxPos*/Op2_FxHeading* ops, inheriting the object's velocity
; In:  a2 = script: word offset to the a4 script; d2 radius, d3 heading (high word), d4/d5 x/y words
; Out: NEXT
; Evidence: dispatched at $5198 (after the moveq #0,d2/d3 of $5194; 127 corpus runs, 155 script uses); $1C2C6 `5294 F6E3 5306 0300 5198 0104`
; ---------------------------------------------------------------------------
Op2_FxSpawnRegs:
	movem.l	$18(a3),d6-d7
loc_00519E:
	move.w	(a2)+,d0
	movem.l	a1-a6,-(a7)
	lea.l	(g_SpawnCtx).w,a6
	movem.l	d6-d7,$3E(a6)
	clr.l	$36(a6)
	clr.l	$3A(a6)
	move.w	$38(a5),$22(a6)
	move.w	$3A(a5),$24(a6)
	lea.l	-$2(a2,d0.w),a4
	movea.w	(a4)+,a0
	swap	d4
	swap	d5
	jsr	(a0)
	movem.l	(a7)+,a1-a6
	movea.w	(a2)+,a0
	jmp	(a0)
