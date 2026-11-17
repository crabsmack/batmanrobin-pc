; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnE - puff plant (Scr2_036512) that waits $2D0 ticks, then every $1E0 ticks puffs 10 puffs with the second puff script ($35714, via $35616)
; In:  d4/d5 = position
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entries $034018, $034042 (Tbl_033F30); TAS f96261-f96325
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnE:
	lea.l	(Scr2_036512).l,a2
	jmp	(Obj_SpawnEnemy).l
