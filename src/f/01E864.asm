; ---------------------------------------------------------------------------
; Boss_Claw_BranchHarleyBeaten - native fragment of the claw script: when g_SceneVars byte 0 is set (Harley's final form beaten, Boss_HarleyJet_SetBeatenFlag) resumes the script at $1E86E (restart coroutines 0/1: the claw leaves), else at $1E87E through Boss_Claw_HarleyNotBeaten
; In:  a2 script context (Op2_Native at $1E862)
; Out: jsr $7FE.w
; Evidence: tst.b g_SceneVars; cleared by Boss_Claw_Spawn, set at TAS f12637 by Boss_HarleyJet_SetBeatenFlag; ran f11992-f12638 (508 frames)
; ---------------------------------------------------------------------------
Boss_Claw_BranchHarleyBeaten:
	tst.b	(g_SceneVars).w
	beq.s	Boss_Claw_HarleyNotBeaten
	jsr	($7FE).w
