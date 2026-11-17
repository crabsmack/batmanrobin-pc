; ---------------------------------------------------------------------------
; Stage2_ArenaVIntUpload - arena V-int upload: interleaves the plane A column V-scroll (g_SceneVars+$A) with plane B's ($F526) into g_SceneWorkBuf, keeps plane B's lower columns ($F54E -> $F576) for the H-int, DMAs line and column scroll and arms Stage2_ArenaHIntSplit at line $F5BF
; In:  a0 = VDP control port
; Out: g_HIntCode = jmp Stage2_ArenaHIntSplit, VDP regs 4/$A/$B/0 set
; Evidence: reached from Stage2_ArenaVIntScroll; Scroll_DMALines + Scroll_DMAColumns, move.l #$245E0,g_HIntOperand; TAS f32066-f35416 every frame
; ---------------------------------------------------------------------------
Stage2_ArenaVIntUpload:
	movem.l	a0-a3,-(a7)
	lea.l	(g_SceneVars+$A).l,a0
	lea.l	(g_SceneVars+$32).l,a1
	lea.l	(g_SceneVars+$5A).l,a2
	lea.l	(g_SceneWorkBuf).l,a3
	lea.l	(g_SceneVars+$82).l,a4
	move.w	#$13,d7
loc_02459C:
	move.w	(a0)+,(a3)+
	move.w	(a1)+,(a3)+
	move.w	(a2)+,(a4)+
	dbra	d7,loc_02459C
	movem.l	(a7)+,a0-a3
	jsr	(Scroll_DMALines).l
; ---------------------------------------------------------------------------
; Stage2_ArenaVIntColumns - the tail of the arena V-int upload, also entered by Stage2_ArenaVIntScroll mid-frame (scroll tables untouched): DMAs the column V-scroll and arms the split H-int Stage2_ArenaHIntSplit at line g_SceneVars2+$21
; In:  a0 = VDP control port, a2 = DMA command scratch
; Out: g_HIntCode = jmp Stage2_ArenaHIntSplit, VDP regs 4/$A/$B/0 set
; Evidence: bne $245B0 in Stage2_ArenaVIntScroll while g_FrameBusy; jsr Scroll_DMAColumns, move.l #$245E0,g_HIntOperand; TAS f32066-f35416
; ---------------------------------------------------------------------------
Stage2_ArenaVIntColumns:
	jsr	(Scroll_DMAColumns).l
	move.w	#$4EF9,(g_HIntCode).l
	move.l	#$245E0,(g_HIntOperand).l
	move.w	#$8A00,d0
	move.b	(g_SceneVars2+$21).w,d0
	beq.s	loc_0245D6
	move.w	#$8407,(a0)
loc_0245D6:
	move.w	d0,(a0)
	move.l	#$8B078014,(a0)
	rts
