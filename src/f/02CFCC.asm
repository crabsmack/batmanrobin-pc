; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnGunshipFast - spawns gunship A's fast, stiff homing missile (Scr2_02CFDC: cruise $500, turn rate $200, body Scr2_0287B0) with heading/straight/homing times from g_VM2Arg0-2
; In:  d4/d5 = x/y; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs
; Evidence: dc.l in the subroutines $2CF1A/$2CF56/$2CF74/$2CFAE (Spawn_AtOwnOfs, offsets ($A, -4/6/$E/-$C)), called by Enemy_Gunship_SpawnA's attack with heading $200, straight 5, homing $B4; TAS 57813-57905
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnGunshipFast:
	lea.l	(Scr2_02CFDC).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
