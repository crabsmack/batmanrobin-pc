; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnDomesTurrets - spawns the dome-cannon mid-boss, second form: the core, two left-facing missile domes behind it at (+$68, -$2E)/(+$68, $2F) and two ball turrets in front (Boss_DomeCannon_SpawnBallTurret, $30/$26 at (-$68, -$2E)/(-$68, $2F))
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2 +$71A; Scr2_031D3C; RAM f72600: core energy 5120 plus two ball turrets (energy $200); TAS f72564
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnDomesTurrets:
	lea.l	(Scr2_031D3C).l,a2
	jmp	(Obj_SpawnEnemy).l
