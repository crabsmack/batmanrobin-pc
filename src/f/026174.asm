; ---------------------------------------------------------------------------
; Boss_Stage2_SpawnTurret3 - spawns turret 3 (Scr2_026180: bit 4, hull offset ($117, $63))
; In:  d4/d5 = position
; Out: d0 = 0, a6 = turret
; Evidence: dc.l at $025C1E (field $22); TAS f32140; RAM f32220: x 194
; ---------------------------------------------------------------------------
Boss_Stage2_SpawnTurret3:
	lea.l	(Scr2_026180).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
