; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnGunshipC - spawns a gunship's homing missile with turn rate $600 (Scr2_02CEDA): the third missile of a fan
; In:  d4/d5 = x/y; g_VM2Arg0-2 = heading, straight frames, homing frames
; Out: as Enemy_Missile_InitFromArgs
; Evidence: dc.l in $2CDCA/$2CE42 (Scr2_02CC5E +$180/+$1F8), offset ($A, 5/6); a second goto Scr2_0287B0 at $2CEF0 is never reached; fuzz runs only
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnGunshipC:
	lea.l	(Scr2_02CEDA).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
