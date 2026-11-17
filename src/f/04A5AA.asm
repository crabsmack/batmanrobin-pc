; ---------------------------------------------------------------------------
; Ending_Epilogue - last ending scene: Batman's face close-up (Lz_0F9352) with the typewriter epilogue ('REVENGE IS A DISH BEST SERVED CHILLED.' ... the laughter of one particular prankster); plain scene init, text font, the narrator object, then the unskippable loop
; In:  none
; Out: via Scene_LoopUnskippable (ends by the narrator's Scene_EndAfterDelay)
; Evidence: third call of Main_Ending; init Scr2_04A52A, a6 script Scr2_04A54E (Lz_0F9352, map, palettes), TextMap_LoadFont d0 = $E42E; scenario-level-skip f19900, png w32 run f20400: Batman's face and the text
; ---------------------------------------------------------------------------
Ending_Epilogue:
	lea.l	Scr2_04A52A(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr2_04A54E(pc),a6
	jsr	(VM6_Run).l
	move.w	#$E42E,d0
	jsr	(TextMap_LoadFont).l
	bsr.w	Ending_SpawnEpilogueNarrator
	bra.w	Scene_LoopUnskippable
