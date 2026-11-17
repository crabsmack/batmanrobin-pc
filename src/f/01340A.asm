; ---------------------------------------------------------------------------
; Item_SpawnBigHealthDrifting - spawn action 32: the large health pickup (+$80 health, sound $41) for the flying sections: drifts sideways at cruise speed $AA (motion coroutine $133CA, body $13316)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 32; Scr2_013416: coroutine 1 = $133CA (Op2_SetAccelXForDist/Op2_SetCruiseX $AA), a larger off-screen box than $132D0; TAS frame 71905 (a flying section, screenshot f71920)
; ---------------------------------------------------------------------------
Item_SpawnBigHealthDrifting:
	lea.l	(Scr2_013416).l,a2
	jmp	(Obj_SpawnEnemy).l
