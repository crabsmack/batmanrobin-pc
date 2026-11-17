; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnBossTripleJetsFar - spawns a gas orb during the Mr. Freeze fight (jittered, 150 px higher) with the jet-sequence / all-three-jets attacks of the faster-puff set ($43F7E / $43EEE)
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_043D54: as Enemy_GasOrb_SpawnBossPairJets with coroutine 1 $43DEC; spawner long at $46F68, $46F8A, $47084 in the Freeze fight scripts; TAS frame 129202, orb with all three jets beside Mr. Freeze in shot w29_s129360
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnBossTripleJetsFar:
	lea.l	(Scr2_043D54).l,a2
	jmp	(Obj_SpawnEnemy).l
