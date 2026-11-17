; ---------------------------------------------------------------------------
; Item_SpawnWeapon0FromHit - spawn action 7: the weapon type 0 pickup (value 1) knocked out of a struck object
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 7; Scr2_013A06 = coroutine $13378, table $13C62 (fixed value 1), then Item_LaunchFromHit; TAS frames 82651-82821
; ---------------------------------------------------------------------------
Item_SpawnWeapon0FromHit:
	lea.l	(Scr2_013A06).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
