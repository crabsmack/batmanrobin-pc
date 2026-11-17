; ---------------------------------------------------------------------------
; Enemy_Clown_SpawnRunIn - spawns a low-energy clown ($20, palette line 3) that does not drop in: it targets a player, runs toward it and leaps (Op2_JumpAnim $46,$180)
; In:  d4/d5 = x/y (Tbl_0166EA +$2B8..+$310)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01B706: coroutine 0 = $1BB34 (BalancePlayerTarget, GosubVector2Plus 4, FaceCruiseToPlayerXTurnBranch, CruiseForward, JumpAnim); a raw spawner for $1B760 (never run) follows at $1B754; TAS f638-f9043
; ---------------------------------------------------------------------------
Enemy_Clown_SpawnRunIn:
	lea.l	(Scr2_01B706).l,a2
	jmp	(Obj_SpawnEnemy).l
