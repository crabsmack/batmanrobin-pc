; ---------------------------------------------------------------------------
; Boss_Stage2_ExplosionDue - carry exit of Boss_Stage2_TickExplosions: resumes at $025D94, which picks one of three hull spots at random (Op2_SwitchRandom) and spawns an explosion effect there
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: bcs target; Scr2_025D94: Op2_SwitchRandom, Op2_FxPosHere + Op2_FxPosAddW ($100,$60 ...) + Op2_FxSpawnRegs; TAS f33636-f34682 (490 frames); explosions on the hull in png f034800
; ---------------------------------------------------------------------------
Boss_Stage2_ExplosionDue:
	jsr	($7FE).w
