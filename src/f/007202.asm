; ---------------------------------------------------------------------------
; VM6_InitLayer - shared tail of the a6 layer-init ops: Layer_Init on layer a1 with the level header from a g_BufPtrs slot and the script's tile word, then continues
; In:  a1 = layer (its +$24 plane VRAM base already set), a6 = script: word g_BufPtrs offset, word tile word
; Out: continues (a6 reloaded from g_A6ScriptPc)
; Evidence: bra from Op6_InitLayerAOnPlaneA/B (and the raw $71E8/$71F6); jsr Layer_Init with a6 = a1, a0 = header, d0 = tile word; TAS f390-90423 (9 scene starts), 21 corpus runs
; ---------------------------------------------------------------------------
VM6_InitLayer:
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	move.w	(a6)+,d0
	move.l	a6,(g_A6ScriptPc).w
	movea.l	a1,a6
	jsr	(Layer_Init).l
	bra.w	VM6_Reload
