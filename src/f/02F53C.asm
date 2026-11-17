; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnFromPodB - spawns a homing missile like Enemy_Missile_SpawnFromPodA with a wider turn (Scr2_02F54C: cruise $300, turn rate $800)
; In:  d4/d5 = position; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in 4 Spawn_AtOwnOfs calls at $2F4AE-$2F50A in the same attack subroutines; jmp Enemy_Missile_InitFromArgs; fuzz-45000-s1/s2 only (f46700-48588)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnFromPodB:
	lea.l	(Scr2_02F54C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
