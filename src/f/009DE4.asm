; ---------------------------------------------------------------------------
; Player_GetP2Var1 - if player 2 is in play, returns its object's script variable 1
; In:  none
; Out: Z clear, d0 = $24(a4) of the object, d1 = 1, a0/a6/a4 set; Z set when not in play
; Evidence: same as Player_GetP1Var1 on g_Player2; only caller SceneCb_WaitPlayersExit; 2490 TAS frames
; ---------------------------------------------------------------------------
Player_GetP2Var1:
	lea.l	(g_Player2).l,a0
	btst	#$0,$25(a0)
	beq.s	loc_009E00
	movea.w	$2C(a0),a6
	movea.w	$A(a6),a4
	move.w	$24(a4),d0
	moveq	#$1,d1
loc_009E00:
	rts
