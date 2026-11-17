; ---------------------------------------------------------------------------
; Scene_EndAfterDelayWhite - a2 native: ends the scene after a delay with a fade to white and a result code
; In:  a2 -> word result, word frames
; Out: g_SceneResult = result, $E034 = frames, g_FrameCallback = SceneCb_EndTimerWhite
; Evidence: scripts $4872E (result 0, 0 frames) and $493C8 via op $2CBC `dc.l $9D24`; ran in idle, scenario-ending, scenario-game-over
; ---------------------------------------------------------------------------
Scene_EndAfterDelayWhite:
	move.w	(a2)+,(g_SceneResult).w
	move.w	(a2)+,d0
	move.w	d0,(g_SceneTimer).w
	move.l	#$9D38,(g_FrameCallback).w
	rts
; ---------------------------------------------------------------------------
; SceneCb_EndTimerWhite - per-frame callback: counts the scene timer down, then starts the fade to white and installs SceneCb_WaitFadeExit
; In:  $E034 = frames left
; Out: $F4B8 = fade target, g_FrameCallback = $9D54
; Evidence: $AA4C sets g_FadeTarget toward +$E (white, CRAM $0EEE at level >= $E in Scene_InitCommon); installed by Scene_EndAfterDelayWhite
; ---------------------------------------------------------------------------
SceneCb_EndTimerWhite:
	subq.w	#$1,(g_SceneTimer).w
	bpl.s	SceneCb_None
	jsr	(Fade_StartOutWhite).l
	move.b	(g_FadeTarget).w,(g_ExitFadeTarget).w
	move.l	#$9D54,(g_FrameCallback).w
	rts
; ---------------------------------------------------------------------------
; SceneCb_WaitFadeExit - per-frame callback: holds the fade target and, when the V-int's fade level reaches it, turns the V-int off and returns from the scene loop to the scene's caller
; In:  $F4B8 = fade target to reach
; Out: on completion g_VIntOff = 1 and two return addresses popped (Frame_End's and the scene loop's)
; Evidence: addq.l #8,a7 then rts: the jsr (a0) in Frame_End and the loop's jsr Frame_End are dropped, so Scene_RunList / Section_RunNext regain control; installed by Scene_FadeOutExit / SceneCb_EndTimerWhite
; ---------------------------------------------------------------------------
SceneCb_WaitFadeExit:
	move.b	#$2,(g_FadeDelay).w
	move.b	(g_ExitFadeTarget).w,d0
	move.b	d0,(g_FadeTarget).w
	cmp.b	(g_FadeLevelVInt).w,d0
	bne.s	loc_009D70
	move.b	#$1,(g_VIntOff).w
	addq.l	#$8,a7
loc_009D70:
	rts
; ---------------------------------------------------------------------------
; SceneCb_CheckGameOver - gameplay per-frame callback: when neither player is in play, locks joining, sets the result to 2 (game over) and arms a $78-frame end timer
; In:  g_Player1/g_Player2 +$25 bit 0
; Out: g_JoinLocked, g_SceneResult = 2, Scene_StartEndTimer(120)
; Evidence: Scene_InitStage installs $9D72; Main_Start treats result 2 as game over (continue screen $4916E); HUD_UnlockJoin also tests for 2
; ---------------------------------------------------------------------------
SceneCb_CheckGameOver:
	btst	#$0,(g_P1Flags).w
	bne.s	loc_009D94
	btst	#$0,(g_P2Flags).w
	bne.s	loc_009D94
	st.b	(g_JoinLocked).w
	move.w	#$2,(g_SceneResult).w
	move.w	#$78,d0
	bra.w	Scene_StartEndTimer
loc_009D94:
	rts
; ---------------------------------------------------------------------------
; SceneCb_WaitPlayersExit - section-exit per-frame callback: counts $EA06 down on frames where every player in play has bit 12 of script variable 1 set, and ends the section when it reaches 0
; In:  $EA06 = frames to count
; Out: Scene_FadeOutExit when done
; Evidence: installed by $1054A after it sets bit $800 in each player's $24(a4) (script variable 1) and g_PlayMode; $EA06 from $10528 ($82) or the script ($10530); Player_GetP1Var1/P2Var1 give the variables
; ---------------------------------------------------------------------------
SceneCb_WaitPlayersExit:
	tst.w	(g_ExitTimer).w
	beq.s	loc_009DC4
	bsr.w	Player_GetP1Var1
	bne.s	loc_009DAA
	bsr.w	Player_GetP2Var1
	bne.s	loc_009DB6
	bra.s	loc_009DC4
loc_009DAA:
	andi.w	#$1000,d0
	beq.s	loc_009DC4
	bsr.w	Player_GetP2Var1
	beq.s	loc_009DBC
loc_009DB6:
	andi.w	#$1000,d0
	beq.s	loc_009DC4
loc_009DBC:
	subq.w	#$1,(g_ExitTimer).w
	ble.w	Scene_FadeOutExit
loc_009DC4:
	rts
