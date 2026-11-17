; ---------------------------------------------------------------------------
; Item_SpawnWeaponDown - spawn action 3: drops the level-down weapon pickup (item value 0, table $13C5C) where the object died (motion coroutine $133AE, then the weapon pickup body $13B6C)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: as Obj_SpawnEnemy (d0 = 0, a6 = the new pickup; nonzero d0 when the pool is short)
; Evidence: Tbl_012EF0 entry 3; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnWeaponDown:
	lea.l	(Scr2_013948).l,a2
	jmp	(Obj_SpawnEnemy).l
