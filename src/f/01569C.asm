; ---------------------------------------------------------------------------
; Debug_PauseMenu - debug pause loop entered from Pause_Update when g_DebugPause is set: A toggles slow motion, B toggles the free camera (placing the players at its cursor when it turns off), C ends the section, Start resumes
; In:  a0 = the pausing player's live pad record (+2 = newly pressed)
; Out: g_FrameBusy cleared; C: $F604.w = 1 then Scene_FadeOutExit
; Evidence: st.b g_FrameBusy while polling; bit 6 not.b g_SlowMotion, bit 4 not.b g_FreeCam, bit 5 Scene_FadeOutExit, bit 7 returns; pad pointer kept in g_DebugPadPtr; jmp from Pause_Update ($9C50) when g_DebugPause != 0; never executed in the corpus (g_DebugPause has no writer but the boot clear)
; ---------------------------------------------------------------------------
Debug_PauseMenu:
	st.b	(g_FrameBusy).w
	move.w	a0,(g_DebugPadPtr).w
	btst	#$6,$2(a0)
	bne.s	loc_0156CA
	btst	#$4,$2(a0)
	bne.s	loc_0156E4
	btst	#$5,$2(a0)
	bne.s	loc_0156D4
	btst	#$7,$2(a0)
	beq.s	Debug_PauseMenu
	sf.b	(g_FrameBusy).w
	rts
loc_0156CA:
	not.b	(g_SlowMotion).w
	sf.b	(g_FrameBusy).w
	rts
loc_0156D4:
	move.w	#$1,(g_FrameCallback).w
	sf.b	(g_FrameBusy).w
	jmp	(Scene_FadeOutExit).l
loc_0156E4:
	sf.b	(g_FrameBusy).w
	not.b	(g_FreeCam).w
	bne.s	Debug_CentreCursor
	lea.l	(g_Player1).l,a1
	bsr.s	Debug_PlacePlayerAtCursor
	lea.l	(g_Player2).l,a1
