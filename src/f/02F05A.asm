; ---------------------------------------------------------------------------
; Boss_TurretPod_CreateTurret2 - creates the turret pod's second turret (Scr2_02F066): energy $800, score $C8, attachment offset (0, -$11), then the turret body at $29174
; In:  d4/d5 = x/y (the pod's)
; Out: as Obj_SpawnBossUnlinked (a6 = the turret)
; Evidence: relative operand of Boss_TurretPod_SpawnTurret2 at $2EA36; the third turret ($2F07C, offset (0, $2B)) follows; TAS f45123
; ---------------------------------------------------------------------------
Boss_TurretPod_CreateTurret2:
	lea.l	(Scr2_02F066).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
