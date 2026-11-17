; ---------------------------------------------------------------------------
; Boss_Claw_HarleyNotBeaten - the not-yet-beaten exit of Boss_Claw_BranchHarleyBeaten: resumes the claw script at $1E87E (its timer-driven attack loop)
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: beq target of Boss_Claw_BranchHarleyBeaten; Scr2_01E87E starts with Op2_TickTimer3Branch; TAS f11992-f12636 (509 frames)
; ---------------------------------------------------------------------------
Boss_Claw_HarleyNotBeaten:
	jsr	($7FE).w
