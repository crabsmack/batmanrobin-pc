; ---------------------------------------------------------------------------
; HUD_Init - scene-start HUD reset: unlocks joining, queues the HUD art upload as the next HUD step and puts both player panels on their opening script
; In:  none
; Out: g_JoinLocked = 0, g_HUDStep = 4, both records via HUD_InitPanel
; Evidence: one caller, the scene setup $99D4 after it reads the scene script; runs at the start of every scene in the corpus (16 TAS frames)
; ---------------------------------------------------------------------------
HUD_Init:
	sf.b	(g_JoinLocked).w
	move.w	#$4,(g_HUDStep).w
	lea.l	(g_Player1).l,a0
	bsr.w	HUD_InitPanel
	lea.l	(g_Player2).l,a0
