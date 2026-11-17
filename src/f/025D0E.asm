; ---------------------------------------------------------------------------
; Boss_Stage2_WaitFloorTurn - native fragment of the boss's phase-2 coroutine 1 (re-run each frame): yields until the arena floor starts turning, then resumes at the descent through Boss_Stage2_FloorTurning
; In:  a2 script context (coroutine 1 slot = $025D0C)
; Out: jsr $7FE.w into $025D18 (Op2_Return) or via Boss_Stage2_FloorTurning
; Evidence: bne to Boss_Stage2_FloorTurning when g_SceneVars2+$12 is nonzero; TAS f33171-f33526 (356 frames, until the tilt at f33526)
; ---------------------------------------------------------------------------
Boss_Stage2_WaitFloorTurn:
	tst.l	(g_SceneVars2+$12).w
	bne.s	Boss_Stage2_FloorTurning
; ---------------------------------------------------------------------------
; Boss_Stage2_FloorWaitYield - the yield of the boss's two floor waits (still turning / not turning yet): back into the coroutine at Scr2_025D18 (Op2_Return), so the wait re-runs next frame
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: bne target of Boss_Stage2_WaitFloorStill and the fall-through of Boss_Stage2_WaitFloorTurn; Scr2_025D18 = Op2_Return; TAS f33171-f33526 (WaitFloorTurn, 356 frames)
; ---------------------------------------------------------------------------
Boss_Stage2_FloorWaitYield:
	jsr	($7FE).w
