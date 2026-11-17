; ---------------------------------------------------------------------------
; Item_SpawnWeaponRandom - spawn action 15: drops a weapon pickup of a random weapon type 0-2 (item value from table $13C74)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 15; Scr2_0139B0 = coroutine $133AE, Item_WeaponItemInit with table $13C74 (3 values 1,2,3); 8 TAS frames (603-111706)
; ---------------------------------------------------------------------------
Item_SpawnWeaponRandom:
	lea.l	(Scr2_0139B0).l,a2
	jmp	(Obj_SpawnEnemy).l
