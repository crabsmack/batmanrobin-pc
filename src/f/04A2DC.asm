; ---------------------------------------------------------------------------
; StageCard_Stage2 - stage 2's card scene (LEVEL TWO / A TWO SIDED STORY): plain scene init (Tbl_04A074) + a6 art script Scr6_04A114, the card director, then the skippable scene loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: second entry of Stage_Intro2's list Tbl_00A172; scenario-level-skip f3835; director Scr2_04A306 spawns the texts $140B2/$140BC
; ---------------------------------------------------------------------------
StageCard_Stage2:
	lea.l	Tbl_04A074(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_04A114(pc),a6
	jsr	(VM6_Run).l
	jsr	(StageCard_SpawnDirector2).l
	bra.w	Scene_LoopSkippable
