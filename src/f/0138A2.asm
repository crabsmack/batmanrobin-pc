; ---------------------------------------------------------------------------
; Item_SpawnRapidChargeDrifting - spawn action 23: the rapid-charge pickup (fast weapon charge for $2D0 frames, sound $48) for the flying sections: drifts sideways at cruise speed $AA (motion coroutine $133CA, body $13316)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 23; Scr2_0138AE: coroutine 1 = $133CA (Op2_SetAccelXForDist/Op2_SetCruiseX $AA), a larger off-screen box than $132D0; 36 TAS frames (36840-128257, stage 2 flying section first)
; ---------------------------------------------------------------------------
Item_SpawnRapidChargeDrifting:
	lea.l	(Scr2_0138AE).l,a2
	jmp	(Obj_SpawnEnemy).l
