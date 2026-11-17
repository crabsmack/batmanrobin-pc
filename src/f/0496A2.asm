; ---------------------------------------------------------------------------
; Title_Run - scene routine of the title screen: plain scene (Tbl_049674 header, Tbl_049686 graphics), the text-map font, the menu director and the piecewise palette fade-in, then the unskippable scene loop $47CB0
; In:  -
; Out: returns through the scene loop: g_SceneResult 0 after 500 idle frames (Main_Title then runs the story list), 1 when START is chosen
; Evidence: only caller Main_Title; TextMap_LoadFont $E3E4; TAS f392, idle every title pass (f1160...); png x-options f420 (the BATMAN & ROBIN logo with the Batman and Robin silhouettes)
; ---------------------------------------------------------------------------
Title_Run:
	lea.l	Tbl_049674(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Tbl_049686(pc),a6
	jsr	(VM6_Run).l
	move.w	#$E3E4,d0
	jsr	(TextMap_LoadFont).l
	bsr.w	Title_SpawnMenu
	bsr.w	Title_StartPaletteFade
	bra.w	Scene_LoopUnskippable
