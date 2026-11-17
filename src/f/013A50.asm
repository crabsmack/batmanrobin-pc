; ---------------------------------------------------------------------------
; Item_SpawnWeaponRandomFromHit - spawn action 16: a random weapon type 0-2 pickup knocked out of a struck object
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 16; Scr2_013A60 = coroutine $13378, table $13C74, then Item_LaunchFromHit; 14 TAS frames (7096-118121)
; ---------------------------------------------------------------------------
Item_SpawnWeaponRandomFromHit:
	lea.l	(Scr2_013A60).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
