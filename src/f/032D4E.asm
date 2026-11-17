; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnBallTurret - spawns a ball turret of the dome cannon's second form as an unlinked child: random speed, ball-turret body $2FDE4 (energy $200/$480); its coroutine $32D64 fires 8 aimed shots $1E0 apart, then swaps target
; In:  d4/d5 = position (the parent's)
; Out: Obj_SpawnEnemyUnlinked's result
; Evidence: dc.l of Op2_SpawnChildOrWait (fields $30/$26) in Scr2_031D3C; RAM f72600: two ball turrets (pc $2FE10, energy 512) beside the core; TAS f72564
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnBallTurret:
	lea.l	(Scr2_032D5A).l,a2
	jmp	(Obj_SpawnEnemyUnlinked).l
