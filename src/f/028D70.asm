; ---------------------------------------------------------------------------
; Boss_TurretPod_ClearTurrets - turret pod mini-boss spawner tail: clears its three turret child fields $1E/$22/$26 before the script spawns them
; In:  a6 = the new pod (after Obj_SpawnBossOrQuit at $2EA1E)
; Out: d0 = 0
; Evidence: jmp from $2EA1E, whose script Scr2_02EA2E then calls Boss_TurretPod_SpawnTurret1-3; TAS f45122 only (the red pod with three twin-barrel turrets, shot f45180)
; ---------------------------------------------------------------------------
Boss_TurretPod_ClearTurrets:
	clr.w	$1E(a6)
	clr.w	$22(a6)
	clr.w	$26(a6)
	moveq	#$0,d0
	rts
