; ---------------------------------------------------------------------------
; VM6_Resume - the a6 VM's resume hook: clears g_A6ResumeHook and continues the loader script at g_A6ScriptPc
; In:  g_A6ScriptPc = script pc
; Out: runs ops until one parks or ends
; Evidence: hook value $70A8 set by Op2_StartA6Script, VM6_Start, Op6_LoadPalettesYield, VM6_RedrawStep; the end of the wait hooks branches here (VM6_WaitBgDecompress, VM6_EndClearJob, VM6_EndBlitJob, VM6_DMAStep); 17 corpus runs
; ---------------------------------------------------------------------------
VM6_Resume:
	clr.l	(g_A6ResumeHook).w
; ---------------------------------------------------------------------------
; VM6_Reload - a6 VM continue after an op that called code clobbering a6: reloads a6 from g_A6ScriptPc and dispatches the next op
; In:  g_A6ScriptPc = pc saved by the op
; Out: falls into VM6_Next
; Evidence: branch target of Op6_Decompress, VM6_InitLayer, Op6_MoveLayerAToCamera, VM6_BlitMapNow and Op6_CallNativeRetry (native returned 0)
; ---------------------------------------------------------------------------
VM6_Reload:
	movea.l	(g_A6ScriptPc).w,a6
; ---------------------------------------------------------------------------
; VM6_Next - a6 VM NEXT: marks the loader script busy (g_A6ScriptBusy) and jumps to the handler word at (a6)+
; In:  a6 = script pc
; Out: jmp to the next op
; Evidence: st g_A6ScriptBusy / movea.w (a6)+,a0 / jmp (a0); the target of every plain op's bra and of Op6_LoadPalettes' pea (its helper rts lands here); 24 corpus runs
; ---------------------------------------------------------------------------
VM6_Next:
	st.b	(g_A6ScriptBusy).w
	movea.w	(a6)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op6_End - a6 op: ends the loader script: clears g_A6ScriptBusy and returns (no hook left, so nothing resumes it)
; In:  no operands
; Out: ends (rts to VM6_Run's or the hook's caller)
; Evidence: word $70B8 ends 30 decoded scripts (e.g. $1FB38 after `70E0 0000 8580 000D`, $1FC4E, $1FD74); Op2_WaitA6Script* wait for g_A6ScriptBusy to clear; 27 corpus runs
; ---------------------------------------------------------------------------
Op6_End:
	sf.b	(g_A6ScriptBusy).w
	rts
; ---------------------------------------------------------------------------
; Op6_CallNativeRetry - a6 op: calls a native routine (dc.l) and continues when it returns d0 = 0; otherwise parks with hook $70D2, which calls the same routine again every frame until it returns 0
; In:  a6 = script: long routine
; Out: continues (d0 = 0) or parks (g_A6ResumeHook = $70D2)
; Evidence: 9 script uses in the stage 2 section scripts, e.g. $33082 `70BE 0003 3356` and $33092 `70BE 0003 337C` (section setup natives ending moveq #0,d0); $70D2 (raw, never needed in the corpus) = movea.l -4(g_A6ScriptPc),a0 / jsr (a0) / beq VM6_Resume; 4 corpus runs
; ---------------------------------------------------------------------------
Op6_CallNativeRetry:
	movea.l	(a6)+,a0
	move.l	a6,(g_A6ScriptPc).w
	jsr	(a0)
	beq.s	VM6_Reload
	move.l	#$70D2,(g_A6ResumeHook).w
	rts
