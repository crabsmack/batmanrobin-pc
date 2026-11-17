; ---------------------------------------------------------------------------
; Boss_TurretPod_Spawn - wave spawner of the turret pod mini-boss: spawns Scr2_02EA2E as a boss-class object (Obj_SpawnBossOrQuit) and clears its turret fields (Boss_TurretPod_ClearTurrets); the script then creates its three turrets
; In:  d4/d5 = x/y
; Out: d0 = 0, or the caller's caller on a full pool
; Evidence: Scr5_021D3A +$2C2 (x $14A, screen y $70, drop action $2E); the script's first ops are Boss_TurretPod_SpawnTurret1-3 natives (op $2CB2) on $2F038/$2F05A/$2F07C; TAS f45122
; ---------------------------------------------------------------------------
Boss_TurretPod_Spawn:
	lea.l	(Scr2_02EA2E).l,a2
	jsr	(Obj_SpawnBossOrQuit).l
	jmp	Boss_TurretPod_ClearTurrets(pc)
