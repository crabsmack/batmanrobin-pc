; ---------------------------------------------------------------------------
; HUD_ShowPressStart - puts one absent player's panel on the PRESS START / GAME OVER loop (Tbl_010FF6); players in play are left alone
; In:  a0 = player record
; Out: +$20 = $10FF6, +$1E = 1 when the player is not in play
; Evidence: the per-record half of HUD_UnlockJoin; decoded script $10FF6; ran on 12 TAS frames (5966-115354)
; ---------------------------------------------------------------------------
HUD_ShowPressStart:
	btst	#$0,$25(a0)
	bne.s	HUD_PressStartReturn
	move.l	#$10FF6,$20(a0)
	move.w	#$1,$1E(a0)
; ---------------------------------------------------------------------------
; HUD_PressStartReturn - the shared rts of HUD_UnlockJoin and HUD_ShowPressStart (frame callback 2, no credits, player in play)
; In:  none
; Out: none
; Evidence: branch target of HUD_UnlockJoin's tests and HUD_ShowPressStart's in-play test
; ---------------------------------------------------------------------------
HUD_PressStartReturn:
	rts
