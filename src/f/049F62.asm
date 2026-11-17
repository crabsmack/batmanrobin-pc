; ---------------------------------------------------------------------------
; Copyright_Run - scene routine of the copyright notice (second attract scene): plain scene (Tbl_049CDC, Scr6_049D38), the text-map font, the notice object, then the scene loop $47C64
; In:  -
; Out: returns after 180 frames or on Start
; Evidence: second entry of Tbl_00A152 (Main_Attract); TextMap_LoadFont $E4E8; TAS f158; png x-attract f200: bat emblem over the DC Comics / SEGA notices
; ---------------------------------------------------------------------------
Copyright_Run:
	lea.l	Tbl_049CDC(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_049D38(pc),a6
	jsr	(VM6_Run).l
	move.w	#$E4E8,d0
	jsr	(TextMap_LoadFont).l
	bsr.w	Copyright_SpawnText
	bra.w	Scene_LoopSkippableDelayed
