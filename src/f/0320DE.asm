; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnFourDomes - spawns the dome-cannon mid-boss, third form: the core, two left-facing missile domes at (+$71, -$2E)/(+$71, $2F) and two right-facing ones (Boss_DomeCannon_SpawnDomeRight, $30/$26 at (-$55, -$2E)/(-$55, $2F))
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2 +$728; Scr2_0320EA; TAS f75007
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnFourDomes:
	lea.l	(Scr2_0320EA).l,a2
	jmp	(Obj_SpawnEnemy).l
