; ---------------------------------------------------------------------------
; Boss_CatHead_DamageSurvived - branch target of Boss_CatHead_TakeDamage when energy is left: re-enters the a2 script at $03B294 (Op2_SetBossEnergy, then the head's frame loop)
; In:  a6 = the head object
; Out: jsr $7FE (script resumes after it)
; Evidence: bgt.s from $03B282; TAS f87527-f89334 (179 frames, one fewer than Boss_CatHead_TakeDamage: the killing hit)
; ---------------------------------------------------------------------------
Boss_CatHead_DamageSurvived:
	jsr	($7FE).w
