; ---------------------------------------------------------------------------
; Stage3_RoadPlaceNear - puts a Hatter-road object nearer than the horizon: its road distance $42(a6) = $840 - the checker phase g_SceneVars2+$E + this frame's speed +$C (Stage3_RoadPlaceAtHorizon with $840)
; In:  a6 = the object
; Out: $42(a6), d0 = 0
; Evidence: move.w #$840,d0 / sub.w g_SceneVars2+$E / add.w g_SceneVars2+$C / move.w d0,$42(a6) / moveq #0,d0 / rts (labelled Scr2_03DDE6 until now: Boss_Hatter_SpawnRoadHazardNear jmps to it); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Stage3_RoadPlaceNear:
	move.w	#$840,d0
	sub.w	(g_SceneVars2+$E).w,d0
	add.w	(g_SceneVars2+$C).w,d0
	move.w	d0,$42(a6)
	moveq	#$0,d0
	rts
