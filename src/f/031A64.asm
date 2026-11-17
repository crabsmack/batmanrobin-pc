; ---------------------------------------------------------------------------
; Enemy_PipeCannon_Spawn5Way - spawns a pipe cannon (patrol as Enemy_PipeCannon_Spawn3Way) that fires only 5-way spreads ($260/$230/$200/$1D0/$1A0), three per cycle $5A0 apart
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2 +$6FE (drop byte $1D); coroutine 1 $31AA0; body Scr2_030416; TAS f74442
; ---------------------------------------------------------------------------
Enemy_PipeCannon_Spawn5Way:
	lea.l	(Scr2_031A70).l,a2
	jmp	(Obj_SpawnEnemy).l
