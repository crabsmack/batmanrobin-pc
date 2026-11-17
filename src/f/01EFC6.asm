; ---------------------------------------------------------------------------
; Boss_HarleyJet_SetBeatenFlag - native fragment at the start of jetpack Harley's defeat: sets the beaten flag g_SceneVars byte 0 (read by Boss_Claw_BranchHarleyBeaten), then resumes at $1EFCE (BossMeter_Defeated, she flies off)
; In:  a2 script context (Op2_Native at $1EFC4)
; Out: jsr $7FE.w
; Evidence: st.b g_SceneVars; TAS f12637 when her energy reached 0 (RAM f12660)
; ---------------------------------------------------------------------------
Boss_HarleyJet_SetBeatenFlag:
	st.b	(g_SceneVars).w
	jsr	($7FE).w
