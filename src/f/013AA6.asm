; ---------------------------------------------------------------------------
; Item_SpawnWeapon0Drifting - spawn action 8: the weapon type 0 pickup (value 1) for the flying sections, drifting sideways
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 8; Scr2_013AB2 = coroutine $133CA, table $13C62, goto $13BC2; 4 TAS frames (37162-50001)
; ---------------------------------------------------------------------------
Item_SpawnWeapon0Drifting:
	lea.l	(Scr2_013AB2).l,a2
	jmp	(Obj_SpawnEnemy).l
