; ---------------------------------------------------------------------------
; Stage1_SpawnTrashCan - trigger callback of the stage 1 street: spawns a breakable trash can (Scr2_020504: energy $20, one frame, explodes with sound $15 and gives its score to the hitter)
; In:  d4/d5 = trigger x/y (Trigger_Fire)
; Out: d0 = 0 when spawned (the entry's top byte becomes its spawn action: the item it drops)
; Evidence: dc.l entries in the street trigger table Tbl_015F5A ($015F66, $015F82, ...); TAS f532-f4237; RAM f600: three Scr2_020504 objects at x 302/323/344 y 181, the three trash cans in shot f600
; ---------------------------------------------------------------------------
Stage1_SpawnTrashCan:
	lea.l	(Scr2_020504).l,a2
	jmp	(Obj_SpawnEnemy).l
