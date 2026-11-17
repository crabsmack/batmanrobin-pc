; ---------------------------------------------------------------------------
; HUD_ShowBossTime - puts one absent player's panel on the BOSS TIME / PLEASE WAIT / credits loop (Tbl_011012); players in play are left alone
; In:  a0 = player record
; Out: +$20 = $11012, +$1E = 1 when the player is not in play
; Evidence: the per-record half of HUD_LockJoin; decoded script $11012; ran in 15 corpus runs, TAS frames 4540-128791
; ---------------------------------------------------------------------------
HUD_ShowBossTime:
	btst	#$0,$25(a0)
	bne.s	HUD_BossTimeReturn
	move.l	#$11012,$20(a0)
	move.w	#$1,$1E(a0)
; ---------------------------------------------------------------------------
; HUD_BossTimeReturn - the shared rts of HUD_LockJoin and HUD_ShowBossTime (no credits / player in play)
; In:  none
; Out: none
; Evidence: branch target of HUD_LockJoin's credit test and HUD_ShowBossTime's in-play test
; ---------------------------------------------------------------------------
HUD_BossTimeReturn:
	rts
; ---------------------------------------------------------------------------
; HUD_UnlockJoin - ends BOSS TIME: unless $F604 = 2, unlocks joining and, when credits are left, puts absent players' panels back on PRESS START (HUD_ShowPressStart)
; In:  none
; Out: g_JoinLocked cleared
; Evidence: reached by bra from $11B06 after it retargets the object in $ED8A ($11B8E handler); executed in the TAS, scenario-ending and fuzz-12600; falls into HUD_ShowPressStart for g_Player2
; ---------------------------------------------------------------------------
HUD_UnlockJoin:
	cmpi.w	#$2,(g_FrameCallback).w
	beq.s	HUD_PressStartReturn
	sf.b	(g_JoinLocked).w
	tst.w	(g_Credits).w
	beq.s	HUD_PressStartReturn
	lea.l	(g_Player1).l,a0
	bsr.w	HUD_ShowPressStart
	lea.l	(g_Player2).l,a0
