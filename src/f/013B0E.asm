; ---------------------------------------------------------------------------
; Item_SpawnWeaponRandomOrDownDrifting - spawn action 20: a weapon pickup with random value 0-3 (table $13C7E; 0 = level down) for the flying sections: drifts sideways (motion coroutine $133CA, body $13BC2)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: as Obj_SpawnEnemy (d0 = 0, a6 = the new pickup; nonzero d0 when the pool is short)
; Evidence: Tbl_012EF0 entry 20; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnWeaponRandomOrDownDrifting:
	lea.l	(Scr2_013B1A).l,a2
	jmp	(Obj_SpawnEnemy).l
