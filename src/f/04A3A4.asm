; ---------------------------------------------------------------------------
; StageCard_Stage3 - stage 3's card scene (LEVEL THREE / TEA TIME!): plain scene init (Tbl_04A050) + a6 art script Scr6_04A138, the card director, then the skippable scene loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: second entry of Stage_Intro3's list Tbl_00A17E; scenario-level-skip / x-skip2 f7525; texts $140CE/$140DA
; ---------------------------------------------------------------------------
StageCard_Stage3:
	lea.l	Tbl_04A050(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_04A138(pc),a6
	jsr	(VM6_Run).l
	bsr.w	StageCard_SpawnDirector3
	bra.w	Scene_LoopSkippable
