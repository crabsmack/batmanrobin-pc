; ---------------------------------------------------------------------------
; Item_SpawnBomb - spawn action 24: drops the bomb pickup (clears the screen, sound $10) where the object died (motion coroutine $133AE, then the common pickup body $132D0)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short (Obj_SpawnEnemy)
; Evidence: Tbl_012EF0 entry 24; Scr2_013712: coroutine 1 = $133AE, touch coroutine at $13776 plays the sound and calls the collect native; 16 TAS frames (1026-127363) and the two-player scenario
; ---------------------------------------------------------------------------
Item_SpawnBomb:
	lea.l	(Scr2_013712).l,a2
	jmp	(Obj_SpawnEnemy).l
