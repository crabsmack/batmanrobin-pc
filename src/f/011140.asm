; ---------------------------------------------------------------------------
; HUD_PlayerOut - switches the HUD when player a0 has lost the last life: GAME OVER on both panels and joining locked when the other player is out too, otherwise GAME OVER then PRESS START/credits (or BOSS TIME while joining is locked)
; In:  a0 = the record of the player who is out
; Out: +$1E/+$20 of one or both records, g_JoinLocked set when both are out
; Evidence: one caller, $1048E after it clears the record's health and lives; scripts $10FD2/$10FD8 (GAME OVER, 1000 frames), $10FE0, $11012, $10FB2; ran in the continue and game-over scenarios (frame 41740)
; ---------------------------------------------------------------------------
HUD_PlayerOut:
	move.w	#$4,$1E(a0)
	lea.l	(g_Player2).l,a1
	cmpa.l	#$FFFFF650,a0
	beq.s	loc_01115A
	lea.l	(g_Player1).l,a1
loc_01115A:
	btst	#$0,$25(a1)
	bne.s	loc_01117E
	move.l	#$10FD2,$20(a0)
	move.l	#$10FD8,$20(a1)
	move.w	#$1,$1E(a1)
	st.b	(g_JoinLocked).w
	rts
loc_01117E:
	tst.w	(g_Credits).w
	beq.s	loc_01119E
	tst.b	(g_JoinLocked).w
	bne.s	loc_011194
	move.l	#$10FE0,$20(a0)
	rts
loc_011194:
	move.l	#$11012,$20(a0)
	rts
loc_01119E:
	move.l	#$10FB2,$20(a0)
	rts
; ---------------------------------------------------------------------------
; HUD_LockJoin - BOSS TIME: locks joining and, when credits are left, puts both records through HUD_ShowBossTime so absent players' panels read BOSS TIME / PLEASE WAIT
; In:  none
; Out: g_JoinLocked set
; Evidence: no static reference (entered from a script or table not yet found); executed in 14 corpus runs (fuzz-30600-s2, fuzz-45000, fuzz-68400-s2, fuzz-70200, fuzz-72000); falls into HUD_ShowBossTime for g_Player2
; ---------------------------------------------------------------------------
HUD_LockJoin:
	st.b	(g_JoinLocked).w
	tst.w	(g_Credits).w
	beq.s	HUD_BossTimeReturn
	lea.l	(g_Player1).l,a0
	bsr.w	HUD_ShowBossTime
	lea.l	(g_Player2).l,a0
