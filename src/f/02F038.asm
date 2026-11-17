; ---------------------------------------------------------------------------
; Boss_TurretPod_CreateTurret1 - creates the turret pod's first turret: an unlinked boss-class object (Obj_SpawnBossUnlinked) with Scr2_02F044: energy $800 (1P and 2P), score $C8, attachment offset (0, 6), then the turret body Scr2_02915A
; In:  d4/d5 = x/y (the pod's)
; Out: as Obj_SpawnBossUnlinked (a6 = the turret)
; Evidence: relative operand of Boss_TurretPod_SpawnTurret1 at $2EA30; Boss_TurretPod_RunTurret adds the offset from its a3 +$14/+$16 each frame; TAS f45123
; ---------------------------------------------------------------------------
Boss_TurretPod_CreateTurret1:
	lea.l	(Scr2_02F044).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
