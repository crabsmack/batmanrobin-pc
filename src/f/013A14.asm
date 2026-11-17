; ---------------------------------------------------------------------------
; Item_SpawnWeapon1FromHit - spawn action 10: the weapon type 1 pickup (value 2, table $13C68) knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x velocity
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 10; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Item_LaunchFromHit as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnWeapon1FromHit:
	lea.l	(Scr2_013A24).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
