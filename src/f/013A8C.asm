; ---------------------------------------------------------------------------
; Item_SpawnWeaponDownDrifting - spawn action 5: the level-down weapon pickup (value 0, table $13C5C) for the flying sections: drifts sideways (motion coroutine $133CA, body $13BC2)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: as Obj_SpawnEnemy (d0 = 0, a6 = the new pickup; nonzero d0 when the pool is short)
; Evidence: Tbl_012EF0 entry 5; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnWeaponDownDrifting:
	lea.l	(Scr2_013A98).l,a2
	jmp	(Obj_SpawnEnemy).l
