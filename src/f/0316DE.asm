; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnSlow - spawns a slower homing missile (Scr2_0316F0: random speed, cruise $255 reached over $60, turn rate $800, body Scr2_0287B0) with heading/straight/homing time from g_VM2Arg0-2
; In:  d4/d5 = position (Spawn_AtOwnOfs, offset (10, 0)); g_VM2Arg0-2
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in the launch subroutine $316CE, gosub'd by Enemy_MissileDome_SpawnFan ($3161C-$31638) and 30 GosubArgs ops in the dome-cannon domes' fan scripts ($32AEA-$32D16); TAS 70742-76015 (8)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnSlow:
	lea.l	(Scr2_0316F0).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Enemy_Missile_InitFromArgs).l
