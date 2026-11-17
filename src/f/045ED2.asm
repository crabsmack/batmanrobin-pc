; ---------------------------------------------------------------------------
; Enemy_GunTank_SpawnTough - spawns a tougher gun tank for the lab's last arena (energy $200, score $190, random speed, cruise $1D5) that fires one 4-shot volley (shots $F0 ticks apart) per round, then waits $3C0 + $B40 ticks
; In:  d4/d5 = x/y (wave entry: x $17C or -$3C)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045EDE: Op2_SetSpeedRand $33/$E6, x accel/cruise $1D5, coroutine 0 $45F06, coroutine 1 $45F1E (wait $B40, counter 1 = 1 so Op2_LoopCounter1 never loops, counter 0 = 4 shots, wait $3C0, goto the $B40 wait), Op2_PlaceYOnScreen $92, body $42FEC; wave entries $41A56/$41A5E (arena wave $41A62, after Op5_LockCameraHere); RAM f120720 an object of this body with slots $45F0E/$45F2A; TAS 120361-120707 (4 spawns)
; ---------------------------------------------------------------------------
Enemy_GunTank_SpawnTough:
	lea.l	(Scr2_045EDE).l,a2
	jmp	(Obj_SpawnEnemy).l
