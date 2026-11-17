; ---------------------------------------------------------------------------
; Item_SpawnRapidCharge - spawn action 21: drops the rapid-charge pickup (fast weapon charge for $2D0 frames, sound $48) where the object died (motion coroutine $133AE, then the common pickup body $132D0)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short (Obj_SpawnEnemy)
; Evidence: Tbl_012EF0 entry 21; Scr2_013882: coroutine 1 = $133AE, touch coroutine at $138CE plays the sound and calls the collect native; 9 TAS frames (1541-127837)
; ---------------------------------------------------------------------------
Item_SpawnRapidCharge:
	lea.l	(Scr2_013882).l,a2
	jmp	(Obj_SpawnEnemy).l
