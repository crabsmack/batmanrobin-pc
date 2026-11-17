; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnTurning - spawns a homing missile with cruise $395 (reached over $60) and turn rate $A00 (coroutine 0 $28836, body Scr2_0287B0), and sets heading/straight/homing time from g_VM2Arg0-2
; In:  d4/d5 = position (Spawn_AtOwnOfs, offset ($A, 0)); g_VM2Arg0-2 from the launcher's GosubArgs
; Out: as Enemy_Missile_InitFromArgs; a short pool: Obj_SpawnEnemyOrQuit returns past this function
; Evidence: Scr2_031772; dc.l of Op2_CallNative Spawn_AtOwnOfs at $031756 in the launch subroutine $031752, which no Gosub, GosubArgs or ROM long names: no reachable caller is known; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnTurning:
	lea.l	(Scr2_031772).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Enemy_Missile_InitFromArgs).l
