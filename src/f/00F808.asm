; ---------------------------------------------------------------------------
; Spr_MaskLeftColumn - when the scene asks for it (g_SceneFixedSpr), blanks the leftmost 16 screen pixels: queues a solid tile block once, then adds a column of 7 high-priority 16x32 sprites at screen x 0 to the SAT buffer every frame
; In:  g_SATPtr / g_SprCountLo = SAT write position and link counter
; Out: 7 SAT entries appended, g_SATPtr and g_SprCountLo advanced; g_LeftMaskArtSent set after the DMA is queued
; Evidence: DMA_Reserve(1, 8), $9380 = $80 words from Art_00F86A (every byte $66) to VRAM $BB00 (tile $5D8); entries y $80+$20n, size $07 (2x4 cells), attr $85D8, x $80; called by Frame_Begin* ($9B86, $9BAC) every frame; TAS RAM: the flag is set in frames 21180-35460 and 101700-108300, the spans that use 2-cell column V-scroll (Scroll_DMAColumns, VInt_ScrollLinesColumns), whose leftmost column the VDP can't scroll cleanly; the black 16-px strip shows in png f024000
; ---------------------------------------------------------------------------
Spr_MaskLeftColumn:
	tst.b	(g_SceneFixedSpr).w
	beq.s	loc_00F868
	tst.b	(g_LeftMaskArtSent).w
	bne.s	loc_00F83C
	moveq	#$1,d0
	moveq	#$8,d1
	bsr.w	DMA_Reserve
	bne.s	loc_00F868
	move.l	#$93809400,(a0)+
	move.l	#$9535967C,(a0)+
	move.w	#$9700,(a0)+
	move.l	#$7B000082,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
	st.b	(g_LeftMaskArtSent).w
loc_00F83C:
	movea.w	(g_SATPtr).w,a0
	move.l	#$600700,d0
	move.b	(g_SprCountLo).w,d0
	move.l	#$85D80080,d1
	moveq	#$6,d7
loc_00F852:
	addi.l	#$200001,d0
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	dbra	d7,loc_00F852
	move.b	d0,(g_SprCountLo).w
	move.w	a0,(g_SATPtr).w
loc_00F868:
	rts
