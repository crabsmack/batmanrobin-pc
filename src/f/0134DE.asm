; ---------------------------------------------------------------------------
; Item_SpawnSmallHealth - spawn action 33: drops a small health pickup where the object died (motion coroutine $133AE, then the LARGE health pickup's body $1341E (+$80, sound $41): its Goto lands on Item_SpawnBigHealth's body, not the small one at $1351E)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: as Obj_SpawnEnemy (d0 = 0, a6 = the new pickup; nonzero d0 when the pool is short)
; Evidence: Tbl_012EF0 entry 33; lea script / jmp Obj_SpawnEnemy as its siblings; script $0134EA: coroutine 1 $133AE, Goto $1341E; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_SpawnSmallHealth:
	lea.l	(Scr2_0134EA).l,a2
	jmp	(Obj_SpawnEnemy).l
