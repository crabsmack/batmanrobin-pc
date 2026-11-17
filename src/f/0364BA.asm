; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnD - puff plant (Scr2_0364C6) that puffs 10 puffs every $2D0 ticks (script $356EE); the never-run spawner $0364E0 (Scr2_0364EC, every $1E0 with the second puff script) follows it, raw
; In:  d4/d5 = position
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry $033FFC (Tbl_033F30); TAS f96209
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnD:
	lea.l	(Scr2_0364C6).l,a2
	jmp	(Obj_SpawnEnemy).l
