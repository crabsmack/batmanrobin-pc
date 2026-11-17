; ---------------------------------------------------------------------------
; Enemy_DropCarrier_SpawnRabbitsC - spawner of a drop carrier that crosses at cruise $255, stops after $780 ticks and then, after a random $3C0-$5DC0 wait and once the population is below 1, drops white-rabbit hoppers (Spawn_AtOwnOfs)
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_036974: Enemy_DropCarrier_SpawnRabbits's opening (energy $100, score $12C, coroutine 1 $035B34, Op2_PlaceYOnScreen) with cruise $255 and coroutine 0 $036B54; the wave spawn entries at $03469A and $0346A2 (x -$28 / $168, y $E06A, action byte $26 on top of the spawner long) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_DropCarrier_SpawnRabbitsC:
	lea.l	(Scr2_036974).l,a2
	jmp	(Obj_SpawnEnemy).l
