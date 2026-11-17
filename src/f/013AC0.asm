; ---------------------------------------------------------------------------
; Item_SpawnWeapon1Drifting - spawn action 11: the weapon type 1 pickup (value 2) for the flying sections, drifting sideways
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 11; Scr2_013ACC = coroutine $133CA, table $13C68 (fixed value 2); 2 fuzz runs only
; ---------------------------------------------------------------------------
Item_SpawnWeapon1Drifting:
	lea.l	(Scr2_013ACC).l,a2
	jmp	(Obj_SpawnEnemy).l
