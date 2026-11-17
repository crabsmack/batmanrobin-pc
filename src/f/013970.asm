; ---------------------------------------------------------------------------
; Item_SpawnWeapon1 - spawn action 9: drops the weapon type 1 pickup (item value 2, table $13C68) where the object died (motion coroutine $133AE, then the weapon pickup body $13B6C)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: as Obj_SpawnEnemy (d0 = 0, a6 = the new pickup; nonzero d0 when the pool is short)
; Evidence: Tbl_012EF0 entry 9; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnWeapon1:
	lea.l	(Scr2_01397C).l,a2
	jmp	(Obj_SpawnEnemy).l
