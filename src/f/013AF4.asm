; ---------------------------------------------------------------------------
; Item_SpawnWeaponRandomDrifting - spawn action 17: a random weapon type 0-2 pickup for the flying sections, drifting sideways
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 17; Scr2_013B00 = coroutine $133CA, table $13C74; 32 TAS frames (36482-107640)
; ---------------------------------------------------------------------------
Item_SpawnWeaponRandomDrifting:
	lea.l	(Scr2_013B00).l,a2
	jmp	(Obj_SpawnEnemy).l
