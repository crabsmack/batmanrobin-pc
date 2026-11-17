; ---------------------------------------------------------------------------
; Scene_InitCommon - resets the machine for a new scene with the display off: clears work RAM, VRAM tables, CRAM and VSRAM, the object lists, pool, boxes and DMA queue, then applies the scene header (section id, art cache size, VDP registers, setup routine, DMA budget, sprite attribute, two mode flags)
; In:  a6 = header: word section id (0 = keep g_SectionId), word art-cache blocks, long VDP register bytes ($AE2C writes 19 regs), long setup routine (jsr), word g_DMABudget, word g_SceneSprAttr, byte $F4C2, byte $F4E8
; Out: a6 past the header; g_FrameCallback = $9CE6 (rts), g_SceneResult = 0; $DEBC = $100, Spr_DrawDirect limits $100/$60/$160/$60
; Evidence: g_VIntOff = 1, VDP reg 0 = $04 / reg 15 = 2; Mem_Clear $FF8000, $75F8 bytes (up to g_VIntCount); $ADFC clears VRAM $BC00, $ADE8 VRAM $0000, CRAM white ($ADBA) when g_FadeLevel >= $E else black ($ADA6), $ADD4 VSRAM; $8B26/$88C2 lists, ObjPool_Init $AD54 x $8C, $8C1A boxes, DMA_ResetQueue, Scene_ResetBufPtrs, $12CC6; callers Scene_InitStage, Scene_InitPlain; 25 TAS frames
; ---------------------------------------------------------------------------
Scene_InitCommon:
	move.b	#$1,(g_VIntOff).w
	st.b	(g_FrameBusy).w
	move.w	#$8004,($C00004).l
	move.w	#$8F02,($C00004).l
	lea.l	(g_ArtHandles&$FFFFFF).l,a0
	move.w	#$75F8,d7
	bsr.w	Mem_Clear
	jsr	(VDP_ClearHScroll).l
	jsr	(VDP_ClearSAT).l
	cmpi.b	#$E,(g_FadeLevel).w
	blt.s	loc_009AF0
	jsr	(VDP_WhiteCRAM).l
	bra.s	loc_009AF6
loc_009AF0:
	jsr	(VDP_ClearCRAM).l
loc_009AF6:
	jsr	(VDP_ClearVSRAM).l
	jsr	(VDP_ClearPlanes).l
	jsr	VIntObj_InitList(pc)
	jsr	Obj_InitList(pc)
	movea.l	#$FFFFAD54,a0
	move.w	#$8C,d0
	jsr	ObjPool_Init(pc)
	jsr	Hitbox_InitLists(pc)
	jsr	DMA_ResetQueue(pc)
	jsr	(Scene_ResetBufPtrs).l
	jsr	(Layer_ResetTileQueue).l
	move.w	(a6)+,d0
	beq.s	loc_009B34
	move.w	d0,(g_SectionId).w
loc_009B34:
	move.w	(a6)+,d1
	jsr	ArtCache_Init(pc)
	movea.l	(a6)+,a0
	jsr	(VDP_LoadRegs).l
	movea.l	(a6)+,a0
	jsr	(a0)
	move.w	(a6)+,(g_DMABudget).w
	move.w	(a6)+,(g_SceneSprAttr).w
	move.b	(a6)+,(g_SceneShadowHL).w
	move.b	(a6)+,(g_SceneFixedSpr).w
	move.l	#$100,(g_WaveTimerStep).w
	move.w	#$100,(g_SprDirectMaxY).w
	move.w	#$60,(g_SprDirectBiasY).w
	move.w	#$160,(g_SprDirectMaxX).w
	move.w	#$60,(g_SprDirectBiasX).w
	move.l	#$9CE6,(g_FrameCallback).w
	move.w	#$0,(g_SceneResult).w
	rts
