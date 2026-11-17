; ---------------------------------------------------------------------------
; Player_SetExitFlag - for one player record in play: sets bit $800 of its script variable 1 (the player scripts walk off) and installs SceneCb_WaitPlayersExit
; In:  a0 = player record (+$2C = object)
; Out: object's a4 +$24 |= $800; g_FrameCallback = $9D96
; Evidence: btst #0,$25(a0); movea.w $2C(a0) / $A(a0); tail of Section_StartExit(N); SceneCb_WaitPlayersExit waits for the bit $1000 answer (its header); 15 corpus runs
; ---------------------------------------------------------------------------
Player_SetExitFlag:
	btst	#$0,$25(a0)
	beq.s	loc_010568
	movea.w	$2C(a0),a0
	movea.w	$A(a0),a0
	ori.w	#$800,$24(a0)
	move.l	#$9D96,(g_FrameCallback).w
loc_010568:
	rts
; ---------------------------------------------------------------------------
; Player_SetBits300All - sets bits $300 in g_PlayMode and in script variable 1 of each player in play
; In:  none
; Out: g_PlayMode |= $300; each object's +$24(a4) |= $300
; Evidence: only caller $2417E (stage 2 section at TAS 32065-35416) when its countdown $F5C0 runs out; Player_CaseStepFrame reads bits $100/$200 as stepping directions; meaning of the pair here not identified
; ---------------------------------------------------------------------------
Player_SetBits300All:
	ori.w	#$300,(g_PlayMode).w
	lea.l	(g_Player1).l,a0
	bsr.w	Player_SetBits300
	lea.l	(g_Player2).l,a0
