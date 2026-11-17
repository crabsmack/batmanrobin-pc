; ---------------------------------------------------------------------------
; HUD_InitPanel - starts one player's HUD panel script: in play shows the stats, otherwise GAME OVER with PRESS START/credits or without credits
; In:  a0 = player record (g_Player1/g_Player2)
; Out: +$1E = 1 (step next frame), +$1A text cleared, +$24/+$3A cleared, +$25 bit 7 set, +$20 = panel script Tbl_010F84 (in play), Tbl_010F8A (credits left) or $10FB6
; Evidence: decoded scripts: $10F84 = show stats, slide in; $10F8A = slide in, ' GAME  OVER ', ' PRESS START', credits loop; $10FB6 = GAME OVER + credits loop; P2 panel shows PRESS START in the 1P TAS (frame 60000)
; ---------------------------------------------------------------------------
HUD_InitPanel:
	move.w	#$1,$1E(a0)
	clr.l	$1A(a0)
	clr.b	$24(a0)
	clr.b	$3A(a0)
	bset	#$7,$25(a0)
	btst	#$0,$25(a0)
	beq.s	loc_0110E6
	move.l	#$10F84,$20(a0)
	rts
loc_0110E6:
	tst.w	(g_Credits).w
	beq.s	loc_0110F6
	move.l	#$10F8A,$20(a0)
	rts
loc_0110F6:
	move.l	#$10FB6,$20(a0)
	rts
