; ---------------------------------------------------------------------------
; Boss_Stage2_StartFight - native fragment of the boss script after the four turrets exist: allows floor tilts (g_Stage2TiltAllowed) and shows the hull picture on plane B (g_Stage2BossPlaneOn), then resumes at $025C38 (music $10, energy $D00/$F80, BossMeter_Show)
; In:  a2 script context (Op2_Native at $025C2A)
; Out: jsr $7FE.w
; Evidence: st.b of both flags; TAS f32140 (one frame), when the hull appears (png f032400)
; ---------------------------------------------------------------------------
Boss_Stage2_StartFight:
	st.b	(g_Stage2TiltAllowed).w
	st.b	(g_Stage2BossPlaneOn).w
	jsr	($7FE).w
