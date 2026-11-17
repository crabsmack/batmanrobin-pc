; ---------------------------------------------------------------------------
; Boss_Stage2_WaitFloorStill - native fragment of the boss's phase-2 coroutine 1: yields while the arena floor is still turning (g_SceneVars2+$12 roll speed nonzero), else goes on to wait for the next turn ($025D0A)
; In:  a2 script context (coroutine 1 slot)
; Out: jsr $7FE.w into $025D18 (Op2_Return: yield) or $025D0A
; Evidence: tst.l g_SceneVars2+$12 (the floor roll speed, Stage2_UpdateScroll); coroutine started at $025CF8 when phase 2 begins; TAS f33171 (one frame)
; ---------------------------------------------------------------------------
Boss_Stage2_WaitFloorStill:
	tst.l	(g_SceneVars2+$12).w
	bne.s	Boss_Stage2_FloorWaitYield
	jsr	($7FE).w
