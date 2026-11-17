; ---------------------------------------------------------------------------
; Boss_Harley_AimNotReached - the not-yet-aimed exit of Boss_Harley_BranchAimReached: resumes the script at $19822
; In:  a2 = script
; Out: script continues at Scr2_019822
; Evidence: jsr $7FE pops $19822; 88 TAS frames f4980-f5074
; ---------------------------------------------------------------------------
Boss_Harley_AimNotReached:
	jsr	($7FE).w
