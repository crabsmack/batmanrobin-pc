; ---------------------------------------------------------------------------
; Pause_Update - in a stage, pauses when a player in play presses Start: holds the main loop until Start again, entering the debug pause menu when $F60E is set, else watching for the section-skip code B A Down B A Down Left Up C
; In:  pads $F62C/$F630 (+2 pressed, +3 released; refreshed by the V-int busy path while paused)
; Out: on the full code: g_JoinLocked set and Scene_FadeOutExit (the section ends); g_FrameBusy set while paused
; Evidence: skipped when g_SectionId ($F602) = 0 or g_FadeLevelVInt < 0; code table Tbl_009CDC = 10 40 02 10 40 02 04 01 20 00 (pad bits B A D B A D L U C, 0 end), the skip NOTES 'Game flow' describes; `move.w #1,$F604` before Scene_FadeOutExit is overwritten by its long write; every frame from f18
; ---------------------------------------------------------------------------
Pause_Update:
	tst.b	(g_SectionId).w
	beq.w	loc_009CDA
	tst.b	(g_FadeLevelVInt).w
	bmi.s	loc_009CDA
	lea.l	(g_PadLive1).w,a0
	lea.l	(g_Player1).w,a1
	btst	#$0,$25(a1)
	beq.s	loc_009C76
	btst	#$7,$2(a0)
	bne.s	loc_009C8E
loc_009C76:
	lea.l	(g_PadLive2).w,a0
	lea.l	(g_Player2).w,a1
	btst	#$0,$25(a1)
	beq.s	loc_009CDA
	btst	#$7,$2(a0)
	beq.s	loc_009CDA
loc_009C8E:
	st.b	(g_FrameBusy).w
loc_009C92:
	btst	#$7,$3(a0)
	beq.s	loc_009C92
	move.w	(g_DebugPause).w,d0
	beq.s	Pause_CheatWaitPress
	jmp	(Debug_PauseMenu).l
; ---------------------------------------------------------------------------
; Pause_CheatRestart - a pause-code mismatch: restarts the comparison at the first entry of Tbl_009CDC with the same press
; In:  d0 = pad press byte
; Out: a1 = Tbl_009CDC, falls into Pause_CheatWaitPress
; Evidence: reached from cmp.b (a1)+,d0 / bne in Pause_Update
; ---------------------------------------------------------------------------
Pause_CheatRestart:
	lea.l	(Tbl_009CDC).l,a1
; ---------------------------------------------------------------------------
; Pause_CheatWaitPress - pause loop body: waits for a press on the pausing pad, unpauses on Start, else checks it against the next code entry
; In:  a0 = pad record, a1 = next code byte
; Out: loops; leaves on Start or a completed code
; Evidence: spins on $2(a0) (pressed byte) with g_FrameBusy set, so only the V-int busy path ($A3D4) updates it; first entry with a1 = the player record mismatches and restarts at Pause_CheatRestart
; ---------------------------------------------------------------------------
Pause_CheatWaitPress:
	move.b	$2(a0),d0
	beq.s	Pause_CheatWaitPress
	btst	#$7,d0
	bne.s	loc_009CD6
	cmp.b	(a1)+,d0
	bne.s	Pause_CheatRestart
loc_009CBC:
	tst.b	$2(a0)
	bne.s	loc_009CBC
	tst.b	(a1)
	bne.s	Pause_CheatWaitPress
	st.b	(g_JoinLocked).w
	move.w	#$1,(g_FrameCallback).w
	jsr	(Scene_FadeOutExit).l
loc_009CD6:
	sf.b	(g_FrameBusy).w
loc_009CDA:
	rts
