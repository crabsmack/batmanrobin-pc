; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverOnPlayer - wave spawner: a red gun drone (Scr2_02B708, body $27F70, random speed) that flies 60 px in, then holds a window of +-10 px round the targeted player for $1E00 ticks, then moves 120 px left and cruises off; its attack waits $6900 ticks, then 4 shot requests in two pairs
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetSpeedRand, Op2_SetVector2HereGotoArgs4 (10, -10, 10, -10) into $27D34 (Op2_SetPlayerWindowFromArgs); entries Scr5_021D3A +$358/+$362/+$510/+$51A, Scr5_023B38 +$82/+$8C; not run in the TAS (fuzz runs only; inferred from the script)
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverOnPlayer:
	lea.l	(Scr2_02B708).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
