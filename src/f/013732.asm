; ---------------------------------------------------------------------------
; Item_SpawnBombDrifting - spawn action 26: the bomb pickup (clears the screen, sound $10) for the flying sections: drifts sideways at cruise speed $AA (motion coroutine $133CA, body $13316)
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: Tbl_012EF0 entry 26; Scr2_01373E: coroutine 1 = $133CA (Op2_SetAccelXForDist/Op2_SetCruiseX $AA), a larger off-screen box than $132D0; 23 TAS frames (39205-128551, stage 2 flying section first)
; ---------------------------------------------------------------------------
Item_SpawnBombDrifting:
	lea.l	(Scr2_01373E).l,a2
	jmp	(Obj_SpawnEnemy).l
