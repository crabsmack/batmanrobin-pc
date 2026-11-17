; ---------------------------------------------------------------------------
; Enemy_PipeCannon_Spawn3Then5Way - spawns a pipe cannon (patrol as Enemy_PipeCannon_Spawn3Way) whose volleys alternate a 3-way spread ($240/$200/$1C0) and, $3C0 later, a 5-way spread ($260/$230/$200/$1D0/$1A0)
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2 +$6F0 (drop byte $17); coroutine 1 $3198C; body Scr2_030416; TAS f72177
; ---------------------------------------------------------------------------
Enemy_PipeCannon_Spawn3Then5Way:
	lea.l	(Scr2_03195C).l,a2
	jmp	(Obj_SpawnEnemy).l
