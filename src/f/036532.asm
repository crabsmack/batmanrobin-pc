; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnF - puff plant (Scr2_03653E) that puffs 10 puffs every $1E0 ticks with the second puff script ($35714)
; In:  d4/d5 = position
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entries $034034, $034050 (Tbl_033F30); TAS f96261-f96325
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnF:
	lea.l	(Scr2_03653E).l,a2
	jmp	(Obj_SpawnEnemy).l
