; ---------------------------------------------------------------------------
; Player_LoseLife - takes a life: with lives left, refills health, halves the weapon level and clears the countdown (d0 = 0); with none, zeroes lives and health and slides the HUD panel out (d0 = 1)
; In:  a6 = player object (+$1E = record)
; Out: d0 = 0 still playing / 1 out of lives
; Evidence: subq.w #1,6(a0) / bmi; lsr.w $2A(a0); HUD_PlayerOut; bsr from Player_HurtFixed and dc.l after op $2D40 in the player scripts ($4B15C, $4CFDC, $4FB18 ...); 119 corpus runs
; ---------------------------------------------------------------------------
Player_LoseLife:
	movea.l	$1E(a6),a0
	subq.w	#$1,$6(a0)
	bmi.s	loc_0104AA
	move.w	#$100,$4(a0)
	lsr.w	$2A(a0)
	clr.w	$26(a0)
	moveq	#$0,d0
	rts
loc_0104AA:
	clr.w	$6(a0)
	clr.w	$4(a0)
	bsr.w	HUD_PlayerOut
	moveq	#$1,d0
	rts
; ---------------------------------------------------------------------------
; Player_CaseSprVar26 - a2 switch native: returns the sprite block's word +$26 (set by the script op $4132) as the switch case
; In:  a5 = sprite block
; Out: d0 = $26(a5)
; Evidence: `move.w $26(a5),d0; rts`; dc.l after op $2D40 at 22 sites $4B91E-$4B9DE in the Batman script; 89 corpus runs
; ---------------------------------------------------------------------------
Player_CaseSprVar26:
	move.w	$26(a5),d0
	rts
; ---------------------------------------------------------------------------
; Player_CaseStepFrame - a2 switch native: picks a stepping animation case: 1-6 cycling (counter $22(a5) += 4 mod $60) when script var 1 has bit $100 while facing left or $200 while facing right, 7-11 cycling (mod $50) for the other bits of $380, 0 when none is set
; In:  a4 = script block (var 1 = $24(a4)), a5 = sprite block (+$38 bit 11 = H flip, +$22 counter)
; Out: d0 = case 0-11; $22(a5) advanced
; Evidence: andi.w #$380; btst #3,$38(a5); lsr.w #4 + 1 / + 7; dc.l after op $2D40 at 18 sites ($4C5CC ... $4F108) in the player scripts; 103 corpus runs
; ---------------------------------------------------------------------------
Player_CaseStepFrame:
	move.w	$24(a4),d0
	andi.w	#$380,d0
	beq.s	loc_0104F0
	btst	#$3,$38(a5)
	beq.s	loc_0104F2
	andi.w	#$100,d0
	beq.s	loc_0104F8
loc_0104D8:
	move.w	$22(a5),d0
	addq.w	#$4,d0
	cmpi.w	#$60,d0
	blt.s	loc_0104E8
	subi.w	#$60,d0
loc_0104E8:
	move.w	d0,$22(a5)
	lsr.w	#$4,d0
	addq.w	#$1,d0
loc_0104F0:
	rts
loc_0104F2:
	andi.w	#$200,d0
	bne.s	loc_0104D8
loc_0104F8:
	move.w	$22(a5),d0
	addq.w	#$4,d0
	cmpi.w	#$50,d0
	blt.s	loc_010508
	subi.w	#$50,d0
loc_010508:
	move.w	d0,$22(a5)
	lsr.w	#$4,d0
	addq.w	#$7,d0
	rts
; ---------------------------------------------------------------------------
; Player_CaseCycle5 - a2 switch native: steps the sprite block counter $24(a5) through 0-9 and returns it halved, a 5-case cycle that changes every 2 calls
; In:  a5 = sprite block
; Out: d0 = 0-4; $24(a5) advanced
; Evidence: addq #1, wrap at $A, lsr #1; dc.l after op $2D40 at $4D5A0 and $500DC; 21 corpus runs
; ---------------------------------------------------------------------------
Player_CaseCycle5:
	move.w	$24(a5),d0
	addq.w	#$1,d0
	cmpi.w	#$A,d0
	blt.s	loc_010520
	moveq	#$0,d0
loc_010520:
	move.w	d0,$24(a5)
	lsr.w	#$1,d0
	rts
; ---------------------------------------------------------------------------
; Section_StartExit - a2 native: starts the end of a section: g_ExitTimer = $82, then as Section_StartExitN
; In:  none
; Out: as Section_StartExitN
; Evidence: dc.l after op $2CBC in $2964E and $3BB80 and after other native ops ($1D318, $343DA, $34866, $41AC0 ...); SceneCb_WaitPlayersExit counts g_ExitTimer; 10 corpus runs
; ---------------------------------------------------------------------------
Section_StartExit:
	move.w	#$82,(g_ExitTimer).w
	bra.s	loc_010534
; ---------------------------------------------------------------------------
; Section_StartExitN - a2 native: starts the end of a section with the timer from the script word: sets g_PlayMode bit $800 and, for each player in play, script var 1 bit $800 and the frame callback SceneCb_WaitPlayersExit
; In:  a2 = script (word operand: exit timer)
; Out: g_ExitTimer = operand; g_FrameCallback = $9D96 when a player is in play
; Evidence: move.w (a2)+,g_ExitTimer; ori.w #$800,g_PlayMode; Player_SetExitFlag for g_Player1 then falls into it for g_Player2; dc.l after op $2CBC at $19A7C, $1D8BC, $1F01E, $265CA, $3C77A, $3DB46, $464B6; 6 corpus runs
; ---------------------------------------------------------------------------
Section_StartExitN:
	move.w	(a2)+,(g_ExitTimer).w
loc_010534:
	ori.w	#$800,(g_PlayMode).w
	lea.l	(g_Player1).l,a0
	bsr.w	Player_SetExitFlag
	lea.l	(g_Player2).l,a0
