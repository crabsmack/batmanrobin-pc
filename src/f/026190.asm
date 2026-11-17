; ---------------------------------------------------------------------------
; Boss_Stage2_SpawnTurret4 - spawns turret 4 (Scr2_02619C: bit 8, hull offset ($147, $63))
; In:  d4/d5 = position
; Out: d0 = 0, a6 = turret
; Evidence: dc.l at $025C26 (field $24); TAS f32140; RAM f32220: x 242 (rightmost)
; ---------------------------------------------------------------------------
Boss_Stage2_SpawnTurret4:
	lea.l	(Scr2_02619C).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
