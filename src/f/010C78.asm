; ---------------------------------------------------------------------------
; Shot_DamageTarget - native hit op of the weapon-shot a4 scripts: marks the struck box hit, gives it the shot's knockback (+/-1.0 by the shot heading) and damage $12(a6), and branches the script on whether the box's energy ran out
; In:  a6 = shot object (+$10 owner id, +$12 damage, +$2A heading 0-$3FF), a2 = struck box, a4 = script after the call (two branch offsets)
; Out: a4 = first offset target when box+$1E dropped to 0 or below, second otherwise; owner id $B boxes always take the second and set bit 15 of $22(a6)
; Evidence: referenced by the a4 native op $5438 at $505D0, $5090C, $50C46 (inside the shot scripts of Tbl_010AF8); executed in 144 corpus runs; box+$1A = attacker id, +$1E owner energy, +$20 damage taken (g_PlayerBoxList layout)
; ---------------------------------------------------------------------------
Shot_DamageTarget:
	bset	#$0,$3C(a2)
	move.w	$10(a6),$1A(a2)
	move.l	#$10000,$22(a2)
	move.w	$2A(a6),d0
	andi.w	#$3FE,d0
	cmpi.w	#$300,d0
	bgt.s	loc_010CA4
	cmpi.w	#$100,d0
	blt.s	loc_010CA4
	neg.l	$22(a2)
loc_010CA4:
	move.w	$12(a6),d0
	add.w	d0,$20(a2)
	cmpi.w	#$B,$10(a2)
	bne.s	loc_010CC4
	ori.w	#$8000,$22(a6)
	sub.w	d0,$1E(a2)
	addq.w	#$2,a4
	adda.w	(a4),a4
	rts
loc_010CC4:
	sub.w	d0,$1E(a2)
	ble.s	loc_010CCC
	addq.w	#$2,a4
loc_010CCC:
	adda.w	(a4),a4
	rts
; ---------------------------------------------------------------------------
; Fx_SpawnSkidEffectTimed - native op: spawns the Fx_SpawnSkidEffect effect every 4th frame (g_FrameCounter & 3 = 0) with no motion test, else skips the 3 operand words
; In:  a6 = object, a2 = operands: word dx, word dy, word jitter range
; Out: a2 past the operands
; Evidence: native callback (a2 op $2CBC) at $357DE and $35B4E; executed only in the TAS; shares the spawn code $10D6C with Fx_SpawnSkidEffect
; ---------------------------------------------------------------------------
Fx_SpawnSkidEffectTimed:
	move.l	(g_FrameCounter).w,d1
	andi.b	#$3,d1
	beq.w	Fx_SpawnSkidAtArgs
; ---------------------------------------------------------------------------
; Fx_SkipSkidArgs - the no-spawn exit of the skid-effect natives: skips their 3 operand words
; In:  a2 = operands
; Out: a2 += 6
; Evidence: addq.l #6,a2 / rts; branch target of Fx_SpawnSkidEffect's speed tests and Fx_SpawnSkidEffectTimed
; ---------------------------------------------------------------------------
Fx_SkipSkidArgs:
	addq.l	#$6,a2
	rts
