; ---------------------------------------------------------------------------
; Item_SpawnWeaponRandomOrDown - spawn action 18: drops a weapon pickup whose value is random among 0-3 (0 = the level-down value of Item_CollectWeapon)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 18; Scr2_0139CA = coroutine $133AE, Item_WeaponItemInit with table $13C7E (4 values 0-3); 2 TAS frames (120197, 121657)
; ---------------------------------------------------------------------------
Item_SpawnWeaponRandomOrDown:
	lea.l	(Scr2_0139CA).l,a2
	jmp	(Obj_SpawnEnemy).l
