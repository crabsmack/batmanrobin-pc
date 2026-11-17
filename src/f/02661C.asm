; ---------------------------------------------------------------------------
; Enemy_LadderThug_PickAnchor - native fragment at the start of the ladder body: while the boss hull is shown (g_Stage2BossPlaneOn) resumes at Enemy_LadderThug_AnchorHull, else at $026626 (ladder from the shaft top at x $A8, y -$F0, moved in screen space)
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: tst.b g_Stage2BossPlaneOn; TAS f24982-f34742 (8 frames, one per ladder); png f025250: a thug on a rope ladder hanging into the lift shaft
; ---------------------------------------------------------------------------
Enemy_LadderThug_PickAnchor:
	tst.b	(g_Stage2BossPlaneOn).w
	bne.s	Enemy_LadderThug_AnchorHull
	jsr	($7FE).w
