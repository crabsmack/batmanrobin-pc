; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnB - puff plant (Scr2_03646E) that first waits $F0 ticks, then every $168 ticks puffs 10 puffs (script $356EE)
; In:  d4/d5 = position
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry $033FA6 (Tbl_033F30); TAS f92084
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnB:
	lea.l	(Scr2_03646E).l,a2
	jmp	(Obj_SpawnEnemy).l
