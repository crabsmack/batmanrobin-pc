; ---------------------------------------------------------------------------
; Item_SpawnSmallHealthDrifting - spawn action 35: the small health pickup (+$10 health, sound $12) for the flying sections: drifts sideways (motion coroutine $133CA, body $1352A)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: as Obj_SpawnEnemy (d0 = 0, a6 = the new pickup; nonzero d0 when the pool is short)
; Evidence: Tbl_012EF0 entry 35; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnSmallHealthDrifting:
	lea.l	(Scr2_013516).l,a2
	jmp	(Obj_SpawnEnemy).l
