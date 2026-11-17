; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnGunshipB - spawns a gunship's homing missile with turn rate $800 (Scr2_02CEB4, otherwise as Enemy_Missile_SpawnGunshipA): the second missile of a fan
; In:  d4/d5 = x/y; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs
; Evidence: dc.l in $2CDAC/$2CE24 (Scr2_02CC5E +$162/+$1DA), offset ($A, -5/-4); fuzz runs only
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnGunshipB:
	lea.l	(Scr2_02CEB4).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
