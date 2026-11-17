; ---------------------------------------------------------------------------
; VM6_LoadPalettes - a6 helper: finds a palette entry in a scene buffer and copies count palettes from it into CRAM lines chosen 2 bits at a time by a mask byte (Pal_LoadLine0-3)
; In:  a6 = script: word g_BufPtrs offset, word entry offset (entry = offset + self-relative long; its first word is skipped), byte count-1, byte mask (bits 7-6 the first palette's line, then 5-4 ...)
; Out: a6 past the 6 operand bytes; g_PalSource lines loaded and flagged for reload
; Evidence: bsr from Op6_LoadPalettesYield / Op6_LoadPalettes; add.b d0,d0 carry pairs pick Pal_LoadLine0/1/2/3 per dbra step; e.g. `7182 0000 0004 0040` = 1 palette to line 1; 32 corpus runs
; ---------------------------------------------------------------------------
VM6_LoadPalettes:
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	adda.w	(a6)+,a0
	adda.l	(a0),a0
	addq.l	#$2,a0
	moveq	#$0,d7
	move.b	(a6)+,d7
	move.b	(a6)+,d0
loc_00719C:
	add.b	d0,d0
	bcs.s	loc_0071B4
	add.b	d0,d0
	bcs.s	loc_0071AC
	jsr	(Pal_LoadLine0).l
	bra.s	loc_0071C6
loc_0071AC:
	jsr	(Pal_LoadLine1).l
	bra.s	loc_0071C6
loc_0071B4:
	add.b	d0,d0
	bcs.s	loc_0071C0
	jsr	(Pal_LoadLine2).l
	bra.s	loc_0071C6
loc_0071C0:
	jsr	(Pal_LoadLine3).l
loc_0071C6:
	dbra	d7,loc_00719C
	rts
; ---------------------------------------------------------------------------
; Op6_InitLayerAOnPlaneA - a6 op: initialises map layer g_LayerA from a level header in a scene buffer, drawn on plane A (layer +$24 = g_PlaneAVRAM)
; In:  a6 = script: word g_BufPtrs offset (level header), word tile word added to every cell
; Out: continues via VM6_InitLayer
; Evidence: 9 script uses, e.g. $1FC30 `71CC 0000 03BA`, $1FD4C `71CC 0000 23DE`; runs at every stage scene start
; ---------------------------------------------------------------------------
Op6_InitLayerAOnPlaneA:
	lea.l	(g_LayerA).l,a1
	move.w	(g_PlaneAVRAM).w,$24(a1)
	bra.s	VM6_InitLayer
; ---------------------------------------------------------------------------
; Op6_InitLayerAOnPlaneB - a6 op: initialises map layer g_LayerA from a level header in a scene buffer, drawn on plane B (layer +$24 = g_PlaneBVRAM)
; In:  a6 = script: word g_BufPtrs offset (level header), word tile word added to every cell
; Out: continues via VM6_InitLayer
; Evidence: 1 script use: $32EB0 `71DA 0000 84A0` (the vertical zeppelin stage $32F14); the never-run ops $71E8/$71F6 do the same for the layer at $EDE8; 3 corpus runs
; ---------------------------------------------------------------------------
Op6_InitLayerAOnPlaneB:
	lea.l	(g_LayerA).l,a1
	move.w	(g_PlaneBVRAM).w,$24(a1)
	bra.s	VM6_InitLayer
