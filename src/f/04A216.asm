; ---------------------------------------------------------------------------
; StageCard_Stage1 - stage 1's card scene (LEVEL ONE / HAPPY BIRTHDAY TO ME! beside the Joker and a burning birthday-cake city): plain scene init (Tbl_04A062) + a6 art script Scr6_04A0F0, the card director object, then the skippable scene loop
; In:  none
; Out: via Scene_LoopSkippable (the director's Scene_EndAfterDelay $C8 or Start ends it)
; Evidence: second entry of Stage_Intro1's scene list Tbl_00A166 (after the shared Batmobile opening BatmobileIntro_Run); scenario-two-player f749; png f840: LEVEL ONE, the Joker, the cake city, HAPPY sliding in
; ---------------------------------------------------------------------------
StageCard_Stage1:
	lea.l	Tbl_04A062(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_04A0F0(pc),a6
	jsr	(VM6_Run).l
	bsr.w	StageCard_SpawnDirector1
	bra.w	Scene_LoopSkippable
