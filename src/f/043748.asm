; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnPairJetsFar - spawns a tube-section gas orb with the two-jet / single-jet sweeps of the faster-puff set (cruise y $1400: $43E1C / $43E66), so its jets reach farther
; In:  d4/d5 = x/y (wave entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_043754: coroutine 1 $437E2 picks vector 1 = $43E1C or $43E66, which set Op2_SetCruiseY $1400 (the puff start speed read by Enemy_GasOrb_StepJet) where the $440FC set uses $A00; wave entries at $41D24/$41D2C; TAS frame 127243
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnPairJetsFar:
	lea.l	(Scr2_043754).l,a2
	jmp	(Obj_SpawnEnemy).l
