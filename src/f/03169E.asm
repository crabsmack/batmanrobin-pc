; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnAgileSlow - spawns a homing missile with Enemy_Missile_SpawnAgile's tight turn (turn rate $A00, coroutine 0 $28836, body Scr2_0287B0) but a lower cruise $355 (reached over $50), and sets heading/straight/homing time from g_VM2Arg0-2
; In:  d4/d5 = position (Spawn_AtOwnOfs, offset ($A, 0)); g_VM2Arg0-2 from the launcher's GosubArgs
; Out: as Enemy_Missile_InitFromArgs; a short pool: Obj_SpawnEnemyOrQuit returns past this function
; Evidence: Scr2_0316B0; dc.l of Op2_CallNative Spawn_AtOwnOfs at $031694 in the launch subroutine $031690, which no Gosub, GosubArgs or ROM long names: no reachable caller is known; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnAgileSlow:
	lea.l	(Scr2_0316B0).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Enemy_Missile_InitFromArgs).l
