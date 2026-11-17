; ---------------------------------------------------------------------------
; Item_SpawnExtraLifeDrifting - spawn action 29: the extra-life pickup (lives +1, sound $42) for the flying sections: drifts sideways at cruise speed $AA (motion coroutine $133CA, body $13316)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 29; Scr2_0135C6: coroutine 1 = $133CA (Op2_SetAccelXForDist/Op2_SetCruiseX $AA), a larger off-screen box than $132D0; TAS frame 74543
; ---------------------------------------------------------------------------
Item_SpawnExtraLifeDrifting:
	lea.l	(Scr2_0135C6).l,a2
	jmp	(Obj_SpawnEnemy).l
