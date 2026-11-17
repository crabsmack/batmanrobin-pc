; ---------------------------------------------------------------------------
; Boss_TurretPod_CreateTurret3 - spawns the turret pod's third turret as an unlinked boss-class object: Scr2_02F088 sets energy $800 (1P/2P), score $C8, the attachment offset (0, $2B) and enters the shared turret body at $29174
; In:  a6 = the pod (d4/d5 position); called through Boss_TurretPod_SpawnTurret3
; Out: Obj_SpawnBossUnlinked's result (a6 = the turret, Z set = spawned)
; Evidence: relative word at $2EA3E in the pod script Scr2_02EA2E (op $2CB2 -> Boss_TurretPod_SpawnTurret3, var 0 bit 4); its siblings $2F038/$2F05A use offsets (0, 6)/(0, -$11); TAS f45123 only (the pod fight 45122-46053)
; ---------------------------------------------------------------------------
Boss_TurretPod_CreateTurret3:
	lea.l	(Scr2_02F088).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
