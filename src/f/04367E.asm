; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnTripleJets - spawns a tube-section gas orb whose attacks are the jet sequence (jets on one by one, then random singles: $4425E) or the all-three-jets sweeps ($441CE), slower puffs (cruise y $A00)
; In:  d4/d5 = x/y (wave entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04368A: as Enemy_GasOrb_SpawnPairJets with coroutine 1 $43718 (attacks $4425E / $441CE); wave entries at $41D14/$41D1C; TAS frame 126197
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnTripleJets:
	lea.l	(Scr2_04368A).l,a2
	jmp	(Obj_SpawnEnemy).l
