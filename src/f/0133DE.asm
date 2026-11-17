; ---------------------------------------------------------------------------
; Item_SpawnBigHealth - spawn action 30: drops the large health pickup (+$80 health, sound $41) where the object died (motion coroutine $133AE, then the common pickup body $132D0)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short (Obj_SpawnEnemy)
; Evidence: Tbl_012EF0 entry 30; Scr2_0133EA: coroutine 1 = $133AE, touch coroutine at $13436 plays the sound and calls the collect native; 4 fuzz runs (not in the TAS)
; ---------------------------------------------------------------------------
Item_SpawnBigHealth:
	lea.l	(Scr2_0133EA).l,a2
	jmp	(Obj_SpawnEnemy).l
