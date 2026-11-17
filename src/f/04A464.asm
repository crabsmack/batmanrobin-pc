; ---------------------------------------------------------------------------
; StageCard_Stage4 - stage 4's card scene (LEVEL FOUR / SNOW IN JULY?): plain scene init (header Scr6_04A098) + a6 art script Scr6_04A15C, the card director, then the skippable scene loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: second entry of Stage_Intro4's list Tbl_00A18A; scenario-level-skip f11953; texts $140E4/$140EF
; ---------------------------------------------------------------------------
StageCard_Stage4:
	lea.l	Scr6_04A098(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_04A15C(pc),a6
	jsr	(VM6_Run).l
	bsr.w	StageCard_SpawnDirector4
	bra.w	Scene_LoopSkippable
