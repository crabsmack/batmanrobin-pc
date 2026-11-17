; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnFromJet - spawns a homing missile (Scr2_029DDC: cruise $2AA, turn rate $700, body Scr2_0287B0) and sets its heading, straight time and homing time from the launcher's g_VM2Arg0-2
; In:  d4/d5 = x/y; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in the jet script at $29DC2 (Spawn_AtOwnOfs, offset $14,0); jmp Enemy_Missile_InitFromArgs; TAS 43185-61879 (16 frames)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnFromJet:
	lea.l	(Scr2_029DDC).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
