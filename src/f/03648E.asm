; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnC - puff plant (Scr2_03649A) that waits $2D0 ticks, then every $2D0 ticks puffs 10 puffs (script $356EE)
; In:  d4/d5 = position
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry $03400A (Tbl_033F30); TAS f96209
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnC:
	lea.l	(Scr2_03649A).l,a2
	jmp	(Obj_SpawnEnemy).l
