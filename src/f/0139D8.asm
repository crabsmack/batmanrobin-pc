; ---------------------------------------------------------------------------
; Item_SpawnWeaponDownFromHit - spawn action 4: the level-down weapon pickup (value 0, table $13C5C) knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x velocity
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 4; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Item_LaunchFromHit as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnWeaponDownFromHit:
	lea.l	(Scr2_0139E8).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
