; ---------------------------------------------------------------------------
; Item_SpawnWeapon2Drifting - spawn action 14: the weapon type 2 pickup (value 3) for the flying sections, drifting sideways
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 14; Scr2_013AE6 = coroutine $133CA, table $13C6E; TAS frame 125959
; ---------------------------------------------------------------------------
Item_SpawnWeapon2Drifting:
	lea.l	(Scr2_013AE6).l,a2
	jmp	(Obj_SpawnEnemy).l
