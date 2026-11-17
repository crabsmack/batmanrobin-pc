; ---------------------------------------------------------------------------
; EnemyAtk_HitScriptSpark - a4 native hit: when the struck box can be hit, adds the script's damage word as a flinch (hit type 6) and puts the script's spark at the contact point; else skips both operands
; In:  a2 = struck box, a4 = script: word damage, long spark a4 script; a6 = attacker, d4-d7 = attack rectangle (for Hitbox_SpawnAtContact)
; Out: a4 past the 6 operand bytes; box +$20 += damage, +$1A = 6; d0 = Hitbox_CanHitTarget's (nonzero: no hit) or Hitbox_SpawnAtContact's
; Evidence: Op4_CallNative's dc.l at $01ED0E (dc.w $20, dc.l Scr4_010964) in the claw fight's falling debris (a4 script $01ECF2: Op4_Move, Op4_BranchTouchPlayer, Op4_BranchCrossY $C3, Op4_DrawKillOffScreen; spawned by Op4_Spawn at $01ECDC, which the claw script's Op2_FxSpawnRegs $0480 at $1E85A starts beside Shake_StartRandom); the native runner stopped at it ('no C for code address $010726') when the debris touched the player as the claw entered (crane-crash 2026-10-30); stepped-gen runs it
; ---------------------------------------------------------------------------
EnemyAtk_HitScriptSpark:
	bsr.w	Hitbox_CanHitTarget
	bne.s	loc_01073E
	move.w	(a4)+,d0
	add.w	d0,$20(a2)
	move.w	#$6,$1A(a2)
	movea.l	(a4)+,a1
	bra.w	Hitbox_SpawnAtContact
loc_01073E:
	addq.w	#$6,a4
	rts
