; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnGunshipA - spawns a gunship's homing missile (Scr2_02CE8E: cruise $2AA, turn rate $A00, body Scr2_0287B0) and sets its heading, straight and homing times from the launcher's g_VM2Arg0-2
; In:  d4/d5 = x/y; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in the gunship subroutines $2CD8E/$2CE06 (Scr2_02CC5E +$144/+$1BC: Spawn_AtOwnOfs, offset ($A, -$A/-$C)); jmp Enemy_Missile_InitFromArgs; fuzz runs only (no launch completed in the TAS)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnGunshipA:
	lea.l	(Scr2_02CE8E).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
