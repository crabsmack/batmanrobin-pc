; ---------------------------------------------------------------------------
; Enemy_HullTurret_SpawnAiming - spawns a hull gun turret that tracks the targeted player (coroutine 0 $2F9EC: keep/pick target, aim, turn) and fires bursts of 5 aimed shots (Fx_SpawnAimedN, a4 script $3142A) at random intervals, then swaps target and pauses; start delay timer 0 = $1E0
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: 10 trigger entries in Tbl_0307C2 (some with drop bytes $11/$17); body $2F98C: energy $40, score $64, map-anchored, 16 barrel directions by heading (frames from $3E1C, rendered: a rotating barrel in a square hull housing); TAS 67688-75161 (7)
; ---------------------------------------------------------------------------
Enemy_HullTurret_SpawnAiming:
	lea.l	(Scr2_030FF4).l,a2
	jmp	(Obj_SpawnEnemy).l
