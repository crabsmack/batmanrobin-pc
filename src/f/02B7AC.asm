; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverOnPlayer - wave spawner: the black-drone version of Enemy_RedDrone_SpawnHoverOnPlayer (Scr2_02B7BC: body $27DBE, then vectors 0/1 jump into Scr2_02B708's course $2B722 and attack $2B764)
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Scr2_02B7BC is 26 bytes: SetSpeedRand, vectors to $2B722/$2B764, goto $27DBE; entries Scr5_021D3A +$36C/+$380/+$3BE/+$3C8; TAS f41698
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverOnPlayer:
	lea.l	(Scr2_02B7BC).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
