; ---------------------------------------------------------------------------
; SegaLogo_Run - scene routine of the SEGA logo (first attract scene, also after GAME OVER): plain scene (Tbl_049CCA header, Scr6_049D08 graphics), the palette-cycling object, then the scene loop $47C64 (Start skips after 30 frames)
; In:  -
; Out: returns after 120 frames (the object's Scene_EndAfterDelay) or on Start
; Evidence: first entry of Tbl_00A152 (Main_Attract); TAS f10, scenario-game-over f43858; png x-attract f100: SEGA logo
; ---------------------------------------------------------------------------
SegaLogo_Run:
	lea.l	Tbl_049CCA(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_049D08(pc),a6
	jsr	(VM6_Run).l
	bsr.w	SegaLogo_SpawnCycler
	bra.w	Scene_LoopSkippableDelayed
