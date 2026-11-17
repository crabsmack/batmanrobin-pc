; ---------------------------------------------------------------------------
; Item_SpawnExtraLife - spawn action 27: drops the extra-life pickup (lives +1, sound $42) where the object died (motion coroutine $133AE, then the common pickup body $132D0)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short (Obj_SpawnEnemy)
; Evidence: Tbl_012EF0 entry 27; Scr2_01359A: coroutine 1 = $133AE, touch coroutine at $135E6 plays the sound and calls the collect native; TAS frame 1299
; ---------------------------------------------------------------------------
Item_SpawnExtraLife:
	lea.l	(Scr2_01359A).l,a2
	jmp	(Obj_SpawnEnemy).l
