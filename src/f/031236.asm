; ---------------------------------------------------------------------------
; Enemy_HullTurret_SpawnSpinning - spawns a hull gun turret that keeps spinning (coroutine 0 = Enemy_HullTurret_SpinStep, 45-degree steps at turn rate $1800) and after a random wait fires 5 shots $28 apart along its current heading, then swaps target
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: 3 trigger entries in Tbl_0307C2 (one with drop byte $11); same body $2F98C; TAS 68185-73727 (2). Raw, never run: a sibling spawner at $0311C6 (Scr2_0311D2) and the unrun turret script Scr2_031068
; ---------------------------------------------------------------------------
Enemy_HullTurret_SpawnSpinning:
	lea.l	(Scr2_031242).l,a2
	jmp	(Obj_SpawnEnemy).l
