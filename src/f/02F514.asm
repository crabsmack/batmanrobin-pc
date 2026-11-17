; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnFromPodA - spawns a homing missile (Scr2_02F524: cruise $300, turn rate $A00, body Scr2_0287B0) and sets heading, straight and homing time from g_VM2Arg0-2
; In:  d4/d5 = position (Spawn_AtOwnOfs); g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs; pops one return address when the pool is short
; Evidence: dc.l in 4 Spawn_AtOwnOfs calls at $2F432-$2F48E (attack subroutines inside the Scr2_02F088 span, entered by GosubArgs ops at $2F36A-$2F420); never in the TAS, only fuzz-45000-s1/s2 (f46640-48558: runs started at the turret pod fight, so the pod's later attacks, inferred)
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnFromPodA:
	lea.l	(Scr2_02F524).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
