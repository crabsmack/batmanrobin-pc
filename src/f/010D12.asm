; ---------------------------------------------------------------------------
; Fx_SpawnSkidEffect - native op: while the mover touches terrain and brakes or reverses at speed, spawns the effect script Scr4_010D9A (or Scr4_010DAC moving left) every 4th frame at an operand offset with random x jitter
; In:  a6 = object, a3 = motion block (+$54 contact flags, +$24 target speed, +$18 + $2 velocity), a2 = operands: word dx, word dy, word jitter range
; Out: a2 past the 6 operand bytes
; Evidence: native callback (a2 op $2CBC) from 9 scripts incl. the players' ($4AD3E, $4D87A); spawns only when |v| > 1.25 with no target, or v opposes the target speed at > 1.5 (skid); g_FrameCounter & 3 = 0; $9982 random; what the effect looks like was not viewed
; ---------------------------------------------------------------------------
Fx_SpawnSkidEffect:
	move.w	$54(a3),d1
	andi.w	#$B,d1
	beq.s	Fx_SkipSkidArgs
; ---------------------------------------------------------------------------
; Fx_SkidFrameTest - Fx_SpawnSkidEffect past its terrain-contact test: on every 4th frame, the speed tests that decide the skid spawn
; In:  as Fx_SpawnSkidEffect
; Out: as Fx_SpawnSkidEffect
; Evidence: a generated-code function after the beq.s to Fx_SkipSkidArgs (the recompiler splits there); g_FrameCounter & 3
; ---------------------------------------------------------------------------
Fx_SkidFrameTest:
	move.l	(g_FrameCounter).w,d1
	andi.b	#$3,d1
	bne.s	Fx_SkipSkidArgs
	tst.l	$24(a3)
	bgt.s	loc_010D48
	blt.s	loc_010D5A
	move.l	$18(a3),d1
	add.l	$2(a3),d1
	cmpi.l	#$14000,d1
	bgt.s	Fx_SpawnSkidAtArgs
	cmpi.l	#$FFFEC000,d1
	blt.s	Fx_SpawnSkidAtArgs
	bra.s	Fx_SkipSkidArgs
loc_010D48:
	move.l	$18(a3),d1
	add.l	$2(a3),d1
	cmpi.l	#$FFFE8000,d1
	blt.s	Fx_SpawnSkidAtArgs
	bra.s	Fx_SkipSkidArgs
loc_010D5A:
	move.l	$18(a3),d1
	add.l	$2(a3),d1
	cmpi.l	#$18000,d1
	blt.w	Fx_SkipSkidArgs
; ---------------------------------------------------------------------------
; Fx_SpawnSkidAtArgs - spawns the skid effect at the operands' offset (dx + random jitter, mirrored with Scr4_010DAC when moving left by d1's sign) from the object's position via Fx_RunScript
; In:  a2 = operands (dx, dy, jitter), d1 = velocity (sign picks the side), a6 = object
; Out: a2 past the operands; d4/d5 = spawn position (high words)
; Evidence: branch target of Fx_SpawnSkidEffect and Fx_SpawnSkidEffectTimed; jmp Fx_RunScript
; ---------------------------------------------------------------------------
Fx_SpawnSkidAtArgs:
	move.w	(a2)+,d4
	move.w	(a2)+,d5
	move.w	(a2)+,d0
	bsr.w	Rand_Signed
	add.w	d0,d4
	lea.l	(Scr4_010D9A).l,a0
	tst.l	d1
	bpl.s	loc_010D8A
	lea.l	(Scr4_010DAC).l,a0
	neg.w	d4
loc_010D8A:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	swap	d4
	swap	d5
	jmp	Fx_RunScript(pc)
