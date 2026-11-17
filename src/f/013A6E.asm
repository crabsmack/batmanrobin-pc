; ---------------------------------------------------------------------------
; Item_SpawnWeaponRandomOrDownFromHit - spawn action 19: a weapon pickup with random value 0-3 knocked out of a struck object
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 19; Scr2_013A7E = coroutine $13378, table $13C7E, then Item_LaunchFromHit; TAS frames 80646-80790
; ---------------------------------------------------------------------------
Item_SpawnWeaponRandomOrDownFromHit:
	lea.l	(Scr2_013A7E).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
