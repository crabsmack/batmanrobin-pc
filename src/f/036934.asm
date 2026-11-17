; ---------------------------------------------------------------------------
; Enemy_DropCarrier_SpawnRabbitsB - spawner of a drop carrier placed on the screen (Op2_PlaceOnScreen) that crosses at cruise $255 and, after $780 ticks, drops white-rabbit hoppers (Spawn_AtOwnOfs) whenever the population is below $F, $5A0 ticks apart
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_036940: Enemy_DropCarrier_SpawnRabbits's opening (energy $100, score $12C, coroutine 1 $035B34) with cruise $255 and coroutine 0 $036A2C (Op2_SwitchRandom, Op2_WaitPopBelowYield0 $F, Op2_CallNative Spawn_AtOwnOfs); the wave spawn entry at $03492E (x 0) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_DropCarrier_SpawnRabbitsB:
	lea.l	(Scr2_036940).l,a2
	jmp	(Obj_SpawnEnemy).l
