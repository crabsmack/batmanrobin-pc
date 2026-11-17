; ---------------------------------------------------------------------------
; Player_SetBits300 - sets bits $300 of one in-play player's script variable 1
; In:  a0 = player record
; Out: +$24(a4) |= $300 when record +$25 bit 0
; Evidence: tail of Player_SetBits300All (bsr for g_Player1, fall-through for g_Player2); TAS frames 33525-35313
; ---------------------------------------------------------------------------
Player_SetBits300:
	btst	#$0,$25(a0)
	beq.s	loc_010596
	movea.w	$2C(a0),a0
	movea.w	$A(a0),a0
	ori.w	#$300,$24(a0)
loc_010596:
	rts
; ---------------------------------------------------------------------------
; Player_ClearBits300All - clears bits $300 in g_PlayMode and in script variable 1 of each player in play (undoes Player_SetBits300All)
; In:  none
; Out: g_PlayMode &= $FCFF; each object's +$24(a4) &= $FCFF
; Evidence: only caller $2417E, right after Shake_StartBig; falls into Player_ClearBits300 for g_Player2
; ---------------------------------------------------------------------------
Player_ClearBits300All:
	andi.w	#$FCFF,(g_PlayMode).w
	lea.l	(g_Player1).l,a0
	bsr.w	Player_ClearBits300
	lea.l	(g_Player2).l,a0
