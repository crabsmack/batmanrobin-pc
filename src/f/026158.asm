; ---------------------------------------------------------------------------
; Boss_Stage2_SpawnTurret2 - spawns turret 2 (Scr2_026164: bit 2, hull offset ($E7, $63))
; In:  d4/d5 = position
; Out: d0 = 0, a6 = turret
; Evidence: dc.l at $025C16 (field $20); TAS f32140; RAM f32220: x 146
; ---------------------------------------------------------------------------
Boss_Stage2_SpawnTurret2:
	lea.l	(Scr2_026164).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
