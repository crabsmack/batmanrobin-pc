; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnGunshipD - spawns a gunship's homing missile with turn rate $800 (Scr2_02CF04): the fourth missile of a fan; its script span also holds the gunship A launch subroutines $2CF1A-$2CFCA
; In:  d4/d5 = x/y; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs
; Evidence: dc.l in $2CDE8/$2CE60 (Scr2_02CC5E +$19E/+$216), offset ($A, $A/$E); fuzz runs only
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnGunshipD:
	lea.l	(Scr2_02CF04).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
