; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnFast - spawns a fast homing missile (Scr2_031732: speed $100, cruise $3D5 reached over $78, turn rate $800, body Scr2_0287B0) with heading/straight/homing time from g_VM2Arg0-2
; In:  d4/d5 = position (Spawn_AtOwnOfs, offset (10, 0)); g_VM2Arg0-2
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in the launch subroutine $31710, gosub'd at $32D3A in the domes' script Scr2_032ACE; TAS 73391-73464 (5). Raw, never run: a fourth missile spawner at $031760 (Scr2_031772)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnFast:
	lea.l	(Scr2_031732).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Enemy_Missile_InitFromArgs).l
