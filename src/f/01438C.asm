; ---------------------------------------------------------------------------
; TextMap_Run - interprets a text-map command list that writes font tiles into the 40x28 plane image g_SceneWorkBuf ($FF9000), or resumes a typewriter text one character later
; In:  a0 = command list (words: offset of the command from TextMap_CmdEnd, then its operands); g_TextMapFont / g_TextMapTileBase from TextMap_LoadFont
; Out: a1-a6 preserved; g_TextMapTyping cleared at the end command
; Evidence: a5 = font map $F3AE, a6 = $FF9000, d6 = tile base $F3B2; jmp TextMap_CmdEnd(pc,d0.w); commands End/Goto/Return/SetPos/Text/Number/MenuItem/BlinkGoto/Typewriter; callers the copyright screen ($49FA0, TAS f164: the DC Comics / SEGA notice), the title menu ($499B6, f404), options ($499E8), $49212 and the ending texts ($4A5E8, $4A734); Tbl_0498EA holds 'OPTIONS', 'MUSIC TEST:'; TAS f164-f406, scenario-options
; ---------------------------------------------------------------------------
TextMap_Run:
	movem.l	a1-a6,-(a7)
	movea.l	(g_TextMapFont).w,a5
	lea.l	(g_SceneWorkBuf).l,a6
	move.w	(g_TextMapTileBase).w,d6
	tst.b	(g_TextMapTyping).w
	bne.s	TextMap_ResumeTyping
TextMap_NextCmd:
	move.w	(a0)+,d0
	jmp	TextMap_CmdEnd(pc,d0.w)
