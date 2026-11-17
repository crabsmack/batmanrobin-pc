; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnBossTripleJets - spawns a gas orb during the Mr. Freeze fight (jittered, 150 px higher) with the jet-sequence / all-three-jets attacks of the slower-puff set ($4425E / $441CE, cruise y $A00)
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_043BAC: as Enemy_GasOrb_SpawnBossPairJets's Scr2_043AD8 with coroutine 1 $43C44 picking vector 1 = $4425E or $441CE; spawner long at $046FF4, $04701E in the Freeze fight scripts; no corpus run reaches it (static, converted with its sibling $043C74)
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnBossTripleJets:
	lea.l	(Scr2_043BAC).l,a2
	jmp	(Obj_SpawnEnemy).l
