; ---------------------------------------------------------------------------
; Stage3_RoadPlaceAtHorizon - puts a Hatter-road object at the road's far end: its road distance $42(a6) = $8C0 - the checker phase g_SceneVars2+$E + this frame's speed +$C, so it lands on a row boundary after the road moves
; In:  a6 = the object
; Out: $42(a6), d0 = 0
; Evidence: jmp from Boss_Hatter_SpawnRoadHazard; Boss_Hatter_RoadHazardPlace kills at distance >= $8C0; the raw code at $03DDE6 (never run) is the same with $840; TAS f114753/f114833
; ---------------------------------------------------------------------------
Stage3_RoadPlaceAtHorizon:
	move.w	#$8C0,d0
	sub.w	(g_SceneVars2+$E).w,d0
	add.w	(g_SceneVars2+$C).w,d0
	move.w	d0,$42(a6)
	moveq	#$0,d0
	rts
