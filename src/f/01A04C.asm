; ---------------------------------------------------------------------------
; Boss_Harley_RumbleWhenFast - while the Harley cart moves faster than $6000 (0.375 px/frame) it kicks a small screen shake every 4-13 frames (script variable 1 counts down)
; In:  a3 = motion block (+$18 x velocity), a4 = script block (+$24 var 1)
; Out: on expiry: g_ShakeVel 2.0, g_ShakeGravity 1.0, g_ShakeY 0, var 1 = 4 + Rand_Below(10)
; Evidence: abs($18(a3)) > $6000; subq.w #1,$24(a4) / bpl; TAS f4540-f4916 (the cart rolling in, 360 frames)
; ---------------------------------------------------------------------------
Boss_Harley_RumbleWhenFast:
	move.l	$18(a3),d0
	bpl.s	loc_01A054
	neg.l	d0
loc_01A054:
	cmpi.l	#$6000,d0
	bgt.s	Boss_Harley_RumbleCountdown
	rts
; Boss_Harley_RumbleCountdown - the countdown and shake without the speed test: also entered here
; directly (fuzz-05400-s1, sync 6,093: the stepped run stopped on this entry having no C)
Boss_Harley_RumbleCountdown:
	subq.w	#$1,$24(a4)
	bpl.s	loc_01A086
	move.l	#$20000,(g_ShakeVel).w
	move.l	#$10000,(g_ShakeGravity).w
	clr.l	(g_ShakeY).w
	moveq	#$A,d0
	jsr	(Rand_Below).l
	addq.w	#$4,d0
	move.w	d0,$24(a4)
loc_01A086:
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_RumbleEvery4 - kicks a small screen shake every 4 frames (script variable 1 counts 3..0), regardless of speed
; In:  a4 = script block
; Out: on expiry: g_ShakeVel 2.0, g_ShakeGravity 1.0, g_ShakeY 0, var 1 = 3
; Evidence: the fixed-period twin of Boss_Harley_RumbleWhenFast; computed entry in 3 fuzz runs (fuzz-05400-s1)
; ---------------------------------------------------------------------------
Boss_Harley_RumbleEvery4:
	subq.w	#$1,$24(a4)
	bpl.s	loc_01A0A8
	move.l	#$20000,(g_ShakeVel).w
	move.l	#$10000,(g_ShakeGravity).w
	clr.l	(g_ShakeY).w
	move.w	#$3,$24(a4)
loc_01A0A8:
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_AimAtPlayer - sets the aim target $24(a5) toward the targeted player (24 directions, stored doubled), or 0 with no targetable player
; In:  a4 +$10 = player record, a6 x/y $3E/$42
; Out: $24(a5) = 2 * Mot_DirToPlayer(24) or 0
; Evidence: tst.b $3A(a0) (targetable), d4 = $18 directions, add.w d2,d2; computed entry in 3 fuzz runs
; ---------------------------------------------------------------------------
Boss_Harley_AimAtPlayer:
	moveq	#$0,d2
	move.w	$10(a4),d0
	beq.s	loc_01A0CC
	movea.w	d0,a0
	tst.b	$3A(a0)
	beq.s	loc_01A0CC
	move.w	$3E(a6),d2
	move.w	$42(a6),d3
	move.w	#$18,d4
	jsr	Mot_DirToPlayer(pc)
	add.w	d2,d2
loc_01A0CC:
	move.w	d2,$24(a5)
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_PickDir8 - picks one of 8 compass directions by script variable 2: 0 gives 0 (up); 1 aims at the targeted player; 2 or more clears the variable and aims up toward the screen centre (7 up-left from the right half, 1 up-right from the left)
; In:  a4 +$26 var 2, +$10 player record; a6 x
; Out: d0 = direction 0-7 (for a native switch op)
; Evidence: Mot_DirToPlayer with d4 = 8; screen x = $3E(a6) - g_CameraX compared with $A0; computed entry in 3 fuzz runs
; ---------------------------------------------------------------------------
Boss_Harley_PickDir8:
	move.w	$26(a4),d0
	beq.s	loc_01A11A
	cmpi.w	#$1,d0
	beq.s	loc_01A0F8
	clr.w	$26(a4)
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmpi.w	#$A0,d0
	blt.s	loc_01A0F4
	moveq	#$7,d0
	rts
loc_01A0F4:
	moveq	#$1,d0
	rts
loc_01A0F8:
	move.w	$10(a4),d0
	beq.s	loc_01A11A
	movea.w	d0,a0
	tst.b	$3A(a0)
	beq.s	loc_01A11A
	move.w	$3E(a6),d2
	move.w	$42(a6),d3
	move.w	#$8,d4
	jsr	Mot_DirToPlayer(pc)
	move.w	d2,d0
	rts
loc_01A11A:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_TurnToAim - turns the aim $22(a5) one step (ring of 48) toward $24(a5) and returns its frame case; while var 0 bit 2 is set it flashes on alternate 2-frame periods (frame case +24, palette line 2 red +2)
; In:  a5 +$22/$24 aim/target, a4 var 0
; Out: d0 = aim/2 (0-23) or +$18 flashing; g_Pal2TintR = 2 or 0
; Evidence: Mot_StepDirToward with d1 = $30; btst #2,$23(a4) and bit 1 of g_VIntCount's low byte; computed entry in 3 fuzz runs
; ---------------------------------------------------------------------------
Boss_Harley_TurnToAim:
	move.w	$22(a5),d0
	move.w	$24(a5),d2
	move.w	#$30,d1
	jsr	Mot_StepDirToward(pc)
	move.w	d0,$22(a5)
	lsr.w	#$1,d0
	btst	#$2,$23(a4)
	beq.s	Boss_Harley_ClearTint
	btst	#$1,($FFFFF5FB).l
	beq.s	Boss_Harley_ClearTint
	addi.w	#$18,d0
	move.b	#$2,(g_Pal2TintR).w
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_ClearTint - clears the red tint of palette line 2 (g_Pal2TintR = 0)
; In:  none
; Out: g_Pal2TintR = 0
; Evidence: tail of Boss_Harley_TurnToAim, also entered directly by the scripts (computed entry after an rts, 3 fuzz runs)
; ---------------------------------------------------------------------------
Boss_Harley_ClearTint:
	clr.b	(g_Pal2TintR).w
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_RollFrame - returns the 2-frame rolling animation case: 0 unless var 0 bit 0; then frame parity (var 0 bit 1) or bit 1 of x (moves with the cart)
; In:  a4 var 0, a6 x
; Out: d0 = 0 / 1
; Evidence: andi.w #1 of g_FrameParity or $3E(a6) >> 1; computed entry in the TAS
; ---------------------------------------------------------------------------
Boss_Harley_RollFrame:
	move.w	$22(a4),d0
	move.w	d0,d1
	andi.w	#$1,d0
	beq.s	loc_01A17C
	andi.w	#$2,d1
	beq.s	loc_01A176
	moveq	#$0,d0
	tst.b	(g_FrameParity).w
	beq.s	loc_01A17C
	moveq	#$1,d0
	bra.s	loc_01A17C
loc_01A176:
	move.w	$3E(a6),d0
	lsr.w	#$1,d0
loc_01A17C:
	andi.w	#$1,d0
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_TintSprite - steps the palette line 3 tint flash and, while it is active, switches the sprite to priority + palette line 3
; In:  a5 = sprite block
; Out: $38(a5) |= $E000 when Tint_StepLine3 returns nonzero
; Evidence: jsr Tint_StepLine3 / ori.w #$E000,$38(a5); computed entry in 3 fuzz runs
; ---------------------------------------------------------------------------
Boss_Harley_TintSprite:
	jsr	(Tint_StepLine3).l
	beq.s	loc_01A190
	ori.w	#$E000,$38(a5)
loc_01A190:
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_PoleAtTarget - returns 1 when the Harley pole's height has reached its target (pole block $10(a6): +0 target, +4 height)
; In:  a6 +$10 = pole block
; Out: d0 = 0 / 1
; Evidence: cmp.w $4(a0),(a0) word compare of the high words; computed entry in the TAS (pole phase)
; ---------------------------------------------------------------------------
Boss_Harley_PoleAtTarget:
	movea.w	$10(a6),a0
	move.w	(a0),d0
	cmp.w	$4(a0),d0
	beq.s	loc_01A1A2
	moveq	#$0,d0
	rts
loc_01A1A2:
	moveq	#$1,d0
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_SetPoleTarget - sets the Harley pole's target height and its acceleration toward it from two script words
; In:  a2 = script: word target height, word acceleration
; Out: pole +0 = target.16, +$C = +-acceleration (sign toward the target); a2 past the words
; Evidence: cmp.l $4(a0),d0 / neg.l d1; Boss_Harley_StepPole adds +$C to the velocity +8; computed entry in fuzz-05400-s2
; ---------------------------------------------------------------------------
Boss_Harley_SetPoleTarget:
	movea.w	$10(a6),a0
	moveq	#$0,d0
	move.w	(a2)+,d0
	swap	d0
	move.w	(a2)+,d1
	ext.l	d1
	cmp.l	$4(a0),d0
	bgt.s	loc_01A1BC
	neg.l	d1
loc_01A1BC:
	move.l	d0,(a0)
	move.l	d1,$C(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_StepPole - moves the Harley pole one frame (velocity += acceleration, height += velocity), bouncing back at a quarter speed when it passes the target and capping it at 130 px, then places the object at the pole top
; In:  a6 +$10 = pole block (+0 target, +4 height, +8 velocity, +$C acceleration; 16.16)
; Out: pole +4/+8 updated; $4E(a6) = x, $50(a6) = y - height
; Evidence: movem.l (a0),d0-d3; overshoot: d1 = target, d2 = -d2/4; cmpi.l #$820000; frame f5850 shows Harley on a raised pole; computed entry in fuzz-05400-s2
; ---------------------------------------------------------------------------
Boss_Harley_StepPole:
	movea.w	$10(a6),a0
	movem.l	(a0),d0-d3
	add.l	d3,d2
	add.l	d2,d1
	tst.l	d3
	bpl.s	loc_01A1DA
	cmp.l	d0,d1
	bgt.s	loc_01A1E4
	bra.s	loc_01A1DE
loc_01A1DA:
	cmp.l	d0,d1
	blt.s	loc_01A1E4
loc_01A1DE:
	move.l	d0,d1
	asr.l	#$2,d2
	neg.l	d2
loc_01A1E4:
	cmpi.l	#$820000,d1
	ble.s	loc_01A1F2
	move.l	#$820000,d1
loc_01A1F2:
	movem.l	d1-d2,$4(a0)
	move.w	$3E(a6),$4E(a6)
; ---------------------------------------------------------------------------
; Boss_Harley_PlaceAtPoleTop - draws the object at the top of the Harley pole: $50(a6) = y - pole height
; In:  a6 +$10 = pole block
; Out: $50(a6) set
; Evidence: computed entry at $1A1FE (inside Boss_Harley_StepPole, after an ordinary move: a script native call) in the TAS
; ---------------------------------------------------------------------------
Boss_Harley_PlaceAtPoleTop:
	movea.w	$10(a6),a0
	move.w	$42(a6),d0
	sub.w	$4(a0),d0
	move.w	d0,$50(a6)
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_ResetPole - retracts the Harley pole: height = target = 20 px, velocity and acceleration 0
; In:  a6 +$10 = pole block
; Out: pole cleared
; Evidence: clr.l $8/$C, move.w #$14 to +4 and +0; computed entry in fuzz-05400-s2
; ---------------------------------------------------------------------------
Boss_Harley_ResetPole:
	movea.w	$10(a6),a0
	clr.l	$8(a0)
	clr.l	$C(a0)
	move.w	#$14,$4(a0)
	move.w	#$14,(a0)
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_PlaceAtPole81 - draws a pole segment at 13/16 of the Harley pole's height: $50(a6) = y - height * $D000 >> 16
; In:  a6 +$10 = pole block shared with the boss
; Out: $50(a6) set
; Evidence: mulu.w #$D000 / swap; a sibling of Boss_Harley_PlaceAtPole62 for the telescoping segments; computed entry in fuzz-05400-s2
; ---------------------------------------------------------------------------
Boss_Harley_PlaceAtPole81:
	movea.w	$10(a6),a0
	move.w	$4(a0),d0
	mulu.w	#$D000,d0
	swap	d0
	neg.w	d0
	add.w	$42(a6),d0
	move.w	d0,$50(a6)
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_PlaceAtPole62 - draws a pole segment at 5/8 of the Harley pole's height: $50(a6) = y - height * $A000 >> 16
; In:  a6 +$10 = pole block shared with the boss
; Out: $50(a6) set
; Evidence: mulu.w #$A000 / swap; computed entry in fuzz-05400-s2
; ---------------------------------------------------------------------------
Boss_Harley_PlaceAtPole62:
	movea.w	$10(a6),a0
	move.w	$4(a0),d0
	mulu.w	#$A000,d0
	swap	d0
	neg.w	d0
	add.w	$42(a6),d0
	move.w	d0,$50(a6)
	rts
; ---------------------------------------------------------------------------
; Boss_Harley_FireAlongAim - spawns the a4 effect script named by the script's next dc.l at the muzzle of the current aim direction (24 offsets 32 px from the centre, with the heading)
; In:  a5 +$22 = aim (0-47), a6 x/y, a2 = script at the dc.l
; Out: via Fx_RunScriptArg (d2 = 0, d3 = heading, d4/d5 = x/y 16.16)
; Evidence: Tbl_01A28E 8 bytes per direction: x, y, heading long (dir 0 = (0, -32) heading $2FF.., dir 6 = (32, 0) heading 0); computed entry in 3 fuzz runs
; ---------------------------------------------------------------------------
Boss_Harley_FireAlongAim:
	move.w	$22(a5),d0
	andi.w	#$FFFE,d0
	add.w	d0,d0
	add.w	d0,d0
	lea.l	(Tbl_01A28E).l,a0
	adda.w	d0,a0
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	add.w	(a0)+,d4
	add.w	(a0)+,d5
	move.l	(a0)+,d3
	moveq	#$0,d2
	swap	d4
	swap	d5
	jmp	(Fx_RunScriptArg).l
