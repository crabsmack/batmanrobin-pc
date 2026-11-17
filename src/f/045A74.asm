; ---------------------------------------------------------------------------
; Enemy_VentTank_SpawnMid - spawns a vent tank (floor height $9E) that cruises at $AA, stops after $78 ticks and alternates vent patterns with rests of $5A0 / $F0 ticks
; In:  d4/d5 = x/y (trigger entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045A80: score $C8, energy $300, coroutine 0 $45A98 (x $AA, chase gosub, wait $78, accel 0, vent gosubs), body $42D74; Tbl_041000 trigger entries x 4448/4704/4960 (spawn action $12); RAM f121500 slots $45ABA/$45B52; TAS 121342-121524 (3 spawns)
; ---------------------------------------------------------------------------
Enemy_VentTank_SpawnMid:
	lea.l	(Scr2_045A80).l,a2
	jmp	(Obj_SpawnEnemy).l
