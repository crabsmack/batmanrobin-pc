; ---------------------------------------------------------------------------
; Enemy_GunTank_SpawnToughFast - spawns a tough lab gun tank (energy $200, score $190) at a random speed ($33-$E6) with cruise $22A, counted in the population
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_045F66: Op2_IncPopulation, speed $33-$E6, score $190, energy $200, x accel/cruise $22A, coroutines 0 $45FC6 and 1 $45FDE, Goto the gun tank body $42FEC; the lab wave's spawn entry at $041756 (x $17C, y $E070) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GunTank_SpawnToughFast:
	lea.l	(Scr2_045F66).l,a2
	jmp	(Obj_SpawnEnemy).l
