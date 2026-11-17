; ---------------------------------------------------------------------------
; Boss_BeamShip_BeamOn - ship native: switches the beam on: script var 0 = the wanted disc radius, palette line 1 colour 15 = the beam colour (reload flagged), coroutine 1 = Scr2_0294A8 (the beam spot leaves the ship and homes on the targeted player)
; In:  a2 = script words radius, colour; a4 = the ship's a4
; Out: a2 past the words
; Evidence: op $2CB2 in the attack scripts $2DFBA, $2E084, $2E144, $2E2B6 ... (e.g. radius $64, colour $A66); var 0 is copied to the radius object by Boss_BeamShip_RunRadiusObj; TAS 77305-78655
; ---------------------------------------------------------------------------
Boss_BeamShip_BeamOn:
	move.w	(a2)+,$22(a4)
	move.w	(a2)+,(g_PalSource1+$1E).w
	st.b	(g_Pal1Reload).w
	move.l	#$294A8,$3C(a4)
	rts
; ---------------------------------------------------------------------------
; Boss_BeamShip_BeamOff - ship native: coroutine 1 = Scr2_0294EC, which zeroes var 0 (the disc shrinks) and brings the beam spot back to the ship
; In:  a4 = the ship's a4
; Out: -
; Evidence: op $2CB2 at $2934C (death) and in the attack scripts $2E120, $2E1A2, $2E258, $2E5A6 ...; TAS frames 77305-78655 (shared ctarget with BeamOn)
; ---------------------------------------------------------------------------
Boss_BeamShip_BeamOff:
	move.l	#$294EC,$3C(a4)
	rts
