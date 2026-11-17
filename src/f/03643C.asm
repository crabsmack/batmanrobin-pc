; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnA - spawner of a puff plant (Scr2_036448, body $0355D4): every $168 ticks it opens and puffs 10 rising puffs (puff script $356EE)
; In:  d4/d5 = position (trigger entry)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entries $033F60-$033F98 in Tbl_033F30 (section $3200, $400E4); TAS f92044-f92084; png f092100: a bulb among leaves at the bottom puffing a column of white balls
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnA:
	lea.l	(Scr2_036448).l,a2
	jmp	(Obj_SpawnEnemy).l
