; ---------------------------------------------------------------------------
; HUD_PlayerJoined - switches the HUD when the player a0 joins mid-game: its panel shows the credits, slides out and back with the stats; the other absent player's panel shows the zero-credit loop when no credits are left
; In:  a0 = the joining player's record
; Out: +$1E/+$20 of both records
; Evidence: one caller, the player object $101CE when Start is pressed and a credit is taken (g_Credits -1); scripts Tbl_010F76 (credits 10, slide out, wait, stats, slide in) and $10FBE; ran in the two-player scenario (frame 1239)
; ---------------------------------------------------------------------------
HUD_PlayerJoined:
	move.w	#$1,$1E(a0)
	move.l	#$10F76,$20(a0)
	lea.l	(g_Player2).l,a1
	cmpa.l	#$FFFFF650,a0
	beq.s	loc_011122
	lea.l	(g_Player1).l,a1
loc_011122:
	btst	#$0,$25(a1)
	bne.s	loc_01113E
	tst.w	(g_Credits).w
	bne.s	loc_01113E
	move.w	#$1,$1E(a1)
	move.l	#$10FBE,$20(a1)
loc_01113E:
	rts
