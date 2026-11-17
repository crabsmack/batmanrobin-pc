; ---------------------------------------------------------------------------
; Boss_Stage2_ShowWreck - native fragment at the end of the boss's defeat (after sound 3 and Fade_FlashWhite6): sets g_Stage2BossPlaneAlt so plane B shows the wrecked hull picture, then resumes at $025DE0 (hull effects, the end ladder)
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: st.b g_Stage2BossPlaneAlt; TAS f34683 (one frame); png f034800/f035400: the wreck with flames
; ---------------------------------------------------------------------------
Boss_Stage2_ShowWreck:
	st.b	(g_Stage2BossPlaneAlt).w
	jsr	($7FE).w
