; ---------------------------------------------------------------------------
; Boss_BeamShip_StartDeathDisc - native fragment in the defeated ship's script: sets palette line 1 colour 15 to white $EEE, gives the radius object the death script Scr2_0295D4 (an expanding disc) and sets the scene flag g_SceneVars+$A2 (draw the disc alone, no beam cone), then re-enters the script
; In:  a6 = the ship; $22(a6) = its radius object
; Out: continues the a2 script after `jsr $7FE.w`
; Evidence: Op2_Native at $29430 after the ship's death sequence (Scr2_02928A: BossMeter_Defeated, explosions); the callback $33A88 tests g_SceneVars+$A2 to draw only Span_BuildCircle + disc; TAS f79013
; ---------------------------------------------------------------------------
Boss_BeamShip_StartDeathDisc:
	move.w	#$EEE,(g_PalSource1+$1E).w
	st.b	(g_Pal1Reload).w
	movea.l	$22(a6),a0
	movea.w	$A(a0),a0
	move.l	#$295D4,$2A(a0)
	st.b	(g_SceneVars+$A2).w
	jsr	($7FE).w
