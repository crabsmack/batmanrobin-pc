; ---------------------------------------------------------------------------
; BatmobileIntro_Run - scene routine of the Batmobile drive shown before every stage (first scene of Stage_Intro1-4's lists): plain scene (Tbl_04A050, Scr6_04A0AA), the animator object, then the Start-skippable scene loop $47C6A
; In:  -
; Out: returns after 120 frames or on Start
; Evidence: first entry of Tbl_00A166/Tbl_00A172/Tbl_00A17E/Tbl_00A18A; TAS f477 (stage 1) ... f116145 (stage 4), 4 times; png x-robin f720: the Batmobile on a streaked road
; ---------------------------------------------------------------------------
BatmobileIntro_Run:
	lea.l	Tbl_04A050(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_04A0AA(pc),a6
	jsr	(VM6_Run).l
	bsr.w	BatmobileIntro_SpawnAnimator
	bra.w	Scene_LoopSkippable
