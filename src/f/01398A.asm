; ---------------------------------------------------------------------------
; Item_SpawnWeapon2 - spawn action 12: drops the weapon pickup of weapon type 2 (item value 3, table $13C6E)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 12; Scr2_013996 = coroutine $133AE, Item_WeaponItemInit with table $13C6E (0, 3: fixed value 3), goto $13B6C (touch -> $13C40); 2 TAS frames (125018, 127253)
; ---------------------------------------------------------------------------
Item_SpawnWeapon2:
	lea.l	(Scr2_013996).l,a2
	jmp	(Obj_SpawnEnemy).l
