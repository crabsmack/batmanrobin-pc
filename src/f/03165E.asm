; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnAgile - spawns a homing missile with a tight turn (Scr2_031670: speed $100, cruise $3AA reached over $50, turn rate $A00, coroutine 0 $28836, body Scr2_0287B0) and sets heading/straight/homing time from g_VM2Arg0-2
; In:  d4/d5 = position (Spawn_AtOwnOfs, offset (13, 0)); g_VM2Arg0-2 from the launcher's GosubArgs
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in the launch subroutine $3164E, gosub'd by Enemy_MissileDome_SpawnSingles ($315CC); not in the TAS, fuzz-68400-s1/s2 (f69175-71368)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnAgile:
	lea.l	(Scr2_031670).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Enemy_Missile_InitFromArgs).l
