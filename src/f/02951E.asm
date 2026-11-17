; ---------------------------------------------------------------------------
; Boss_BeamShip_SetSpotVertex - native fragment in the beam-spot coroutine: while the spot's position is swapped into $3E/$42 (Op2_SwapSavedPos), stores it as span vertex 1 (g_TriX1/Y1, the disc centre), then swaps back
; In:  a6 = the ship with the spot in $3E/$42
; Out: g_TriX1/g_TriY1
; Evidence: Op2_Native at $2951C in Scr2_0294A8/0294EC after the spot's move; $33A88 draws the beam cone to (g_TriX1, g_TriY1) and the disc there; TAS 77306-79013 (1663 frames)
; ---------------------------------------------------------------------------
Boss_BeamShip_SetSpotVertex:
	move.w	$3E(a6),(g_TriX1).w
	move.w	$42(a6),(g_TriY1).w
	jsr	($7FE).w
