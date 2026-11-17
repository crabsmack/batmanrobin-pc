; ---------------------------------------------------------------------------
; Flight_DrawSky - plane B sky of the flying section after the corridor: V-scroll = 7/8 of g_ViewY; H-scroll fills 224 lines from the long g_SceneVars+$9C whose two words drift -7 and -3 px a frame, alternating line by line (two interleaved cloud layers), swapped on alternate frames
; In:  -
; Out: g_VScrollB, g_HScrollLinesB (112 longs via Mem_FillLongs)
; Evidence: frame callback (Flight_StartSkyOnly) and called by Flight_FrameAirship, Flight_FrameBeamShip and the Flight_FrameClouds tail; TAS f47955-f79303; png f051000/f064200: the streaked purple sky
; ---------------------------------------------------------------------------
Flight_DrawSky:
	move.w	(g_ViewY).w,d0
	asr.w	#$1,d0
	move.w	d0,d2
	asr.w	#$1,d0
	add.w	d0,d2
	asr.w	#$1,d0
	add.w	d0,d2
	move.w	d2,(g_VScrollB).w
	move.l	(g_SceneVars+$9C).l,d0
	subq.w	#$3,d0
	swap	d0
	subq.w	#$7,d0
	swap	d0
	move.l	d0,(g_SceneVars+$9C).w
	move.b	(g_FrameParity).w,d3
	eor.b	d2,d3
	lsr.b	#$1,d3
	bcc.s	loc_03347A
	swap	d0
loc_03347A:
	lea.l	(g_HScrollLinesB).l,a0
	move.w	#$70,d7
	jmp	(Mem_FillLongs).l
; ---------------------------------------------------------------------------
; Flight_FrameCorridorClouds - the corridor-exit frame's two-layer part: Flight_UpdateCorridor (the plane B corridor), then on into Flight_ScrollCloudBank (the plane A cloud bank)
; In:  -
; Out: as the two callees: g_HScrollLinesA/B, g_VScrollA/B
; Evidence: blt.w from Flight_FrameCorridorToClouds while g_LayerA x < $680; jsr Flight_UpdateCorridor, then the fall-through into Flight_ScrollCloudBank; TAS f46291-f47955
; ---------------------------------------------------------------------------
Flight_FrameCorridorClouds:
	jsr	(Flight_UpdateCorridor).l
