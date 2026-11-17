; ---------------------------------------------------------------------------
; VM6_SetupMapBlit - a6 helper: sets up a cell-map blit from a buffer entry (word width, word height, then cell words): source row pointer and stride, tile word, the VDP write command of the destination and the column/row counts (clipped to the plane)
; In:  d0 = src x (bytes), d1 = src row, d2 = dest x (bytes), d3 = dest row, d4/d5 = width/height (0 = the map's); a6 = script: word g_BufPtrs offset, word entry offset (self-relative long), word address of the plane VRAM variable (added to d2), word tile word
; Out: g_A6MapStride = map width * 2, g_A6MapRowPtr = map + d0 + d1 * stride, g_A6TileWord, g_A6VdpCmd, g_A6RectCols/Rows = width-1/height-1; g_A6ScriptPc = a6 past the operands
; Evidence: callers Op6_DrawMap/DrawMapRect/DrawMapVInt/DrawMapRectVInt; e.g. `7468 0000 0000 EE36 02D2` = buffer 0 entry 0, plane B, + $2D2; 34 corpus runs
; ---------------------------------------------------------------------------
VM6_SetupMapBlit:
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	adda.w	(a6)+,a0
	adda.l	(a0),a0
	movea.w	(a6)+,a1
	add.w	(a1),d2
	move.w	(a6)+,(g_A6TileWord).w
	moveq	#$0,d6
	move.w	(a0)+,d6
	move.w	(a0)+,d7
	tst.w	d4
	bne.s	loc_0074CA
	move.w	d6,d4
loc_0074CA:
	tst.w	d5
	bne.s	loc_0074D0
	move.w	d7,d5
loc_0074D0:
	add.w	d6,d6
	move.l	d6,(g_A6MapStride).w
	move.w	(g_PlaneCellsX).l,d7
	sub.w	d4,d7
	sub.w	d2,d7
	bpl.s	loc_0074E4
	add.w	d7,d4
loc_0074E4:
	move.w	(g_PlaneCellsY).l,d7
	sub.w	d5,d7
	sub.w	d3,d7
	bpl.s	loc_0074F2
	add.w	d7,d5
loc_0074F2:
	subq.w	#$1,d4
	move.w	d4,(g_A6RectCols).w
	subq.w	#$1,d5
	move.w	d5,(g_A6RectRows).w
	adda.w	d0,a0
	mulu.w	d6,d1
	adda.l	d1,a0
	move.l	a0,(g_A6MapRowPtr).w
	mulu.w	(g_PlaneRowBytes).w,d3
	add.w	d2,d3
	lsl.l	#$2,d3
	lsr.w	#$2,d3
	ori.w	#$4000,d3
	swap	d3
	move.l	d3,(g_A6VdpCmd).w
	move.l	a6,(g_A6ScriptPc).w
	rts
; ---------------------------------------------------------------------------
; VM6_BlitMapNow - a6: writes the set-up map rectangle straight to the VDP data port row by row (each cell + the tile word), then continues the script
; In:  g_A6MapRowPtr, g_A6MapStride, g_A6TileWord, g_A6VdpCmd, g_A6RectCols/Rows
; Out: continues (VM6_Reload)
; Evidence: bra from Op6_DrawMap/Op6_DrawMapRect; writes $C00004/$C00000 directly (scene setup, before the display is on); 24 corpus runs
; ---------------------------------------------------------------------------
VM6_BlitMapNow:
	lea.l	($C00004).l,a0
	lea.l	-$4(a0),a1
	move.w	(g_A6TileWord).w,d0
	move.l	(g_A6VdpCmd).w,d1
	moveq	#$0,d2
	move.w	(g_PlaneRowBytes).w,d2
	lsl.l	#$2,d2
	move.w	(g_A6RectRows).w,d7
loc_007540:
	move.l	d1,(a0)
	movea.l	(g_A6MapRowPtr).w,a3
	move.w	(g_A6RectCols).w,d6
loc_00754A:
	move.w	(a3)+,d3
	add.w	d0,d3
	move.w	d3,(a1)
	dbra	d6,loc_00754A
	move.l	(g_A6MapStride).w,d6
	add.l	d6,(g_A6MapRowPtr).w
	swap	d1
	rol.w	#$2,d1
	add.l	d2,d1
	ror.w	#$2,d1
	swap	d1
	dbra	d7,loc_007540
	bra.w	VM6_Reload
; ---------------------------------------------------------------------------
; VM6_StartBlitJob - a6: starts the map-blit V-int job: allocates a V-int list object (retrying each frame while the pool is full) with handler VM6_VIntBlitRows, and parks the script
; In:  the blit set up by VM6_SetupMapBlit
; Out: parks: g_A6ResumeHook = 0 (or $756E to retry), g_A6VIntJob = the object
; Evidence: jsr $8BB8; move.l #$759E,(a6); bra from Op6_DrawMapVInt/DrawMapRectVInt; 9 corpus runs
; ---------------------------------------------------------------------------
VM6_StartBlitJob:
	jsr	(VIntObj_NewIdle).l
	beq.s	loc_007580
	move.l	#$756E,(g_A6ResumeHook).w
	rts
loc_007580:
	clr.l	(g_A6ResumeHook).w
	move.w	a6,(g_A6VIntJob).w
	move.l	#$759E,(a6)
	rts
; ---------------------------------------------------------------------------
; VM6_EndBlitJob - a6 resume hook after a map blit: frees the V-int job object and resumes the script
; In:  g_A6VIntJob = the job object
; Out: VM6_Resume
; Evidence: hook value $7590 set by VM6_VIntBlitRows after the last row; jsr $8BF6; 9 corpus runs
; ---------------------------------------------------------------------------
VM6_EndBlitJob:
	movea.w	(g_A6VIntJob).w,a6
	jsr	(VIntObj_UnlinkFree).l
	bra.w	VM6_Resume
; ---------------------------------------------------------------------------
; VM6_VIntBlitRows - V-int list handler of the map-blit job: writes map rows to the VDP while the DMA budget (cells) lasts; after the last row it idles and hands the end to Frame_End
; In:  a6 = job object, a0 = $C00004, a1 = $C00000 (V-int registers); the blit state g_A6*
; Out: g_A6VdpCmd/g_A6MapRowPtr advanced; when done: handler = $75FC (rts), g_A6ResumeHook = VM6_EndBlitJob
; Evidence: run from $8ADA in the V-int, skips while g_FrameBusy; d7 = g_DMABudgetLeft minus (cols + 1) per row; 9 corpus runs
; ---------------------------------------------------------------------------
VM6_VIntBlitRows:
	tst.b	(g_FrameBusy).w
	bne.s	VM6_BlitJobIdle
	move.w	(g_A6TileWord).w,d0
	move.l	(g_A6VdpCmd).w,d1
	moveq	#$0,d2
	move.w	(g_PlaneRowBytes).w,d2
	lsl.l	#$2,d2
	move.w	(g_DMABudgetLeft).w,d7
	move.w	(g_A6RectCols).w,d6
	addq.w	#$1,d6
loc_0075BE:
	sub.w	d6,d7
	ble.s	loc_0075FE
	move.l	d1,(a0)
	movea.l	(g_A6MapRowPtr).w,a3
	move.w	(g_A6RectCols).w,d5
loc_0075CC:
	move.w	(a3)+,d3
	add.w	d0,d3
	move.w	d3,(a1)
	dbra	d5,loc_0075CC
	move.l	(g_A6MapStride).w,d5
	add.l	d5,(g_A6MapRowPtr).w
	swap	d1
	rol.w	#$2,d1
	add.l	d2,d1
	ror.w	#$2,d1
	swap	d1
	subq.w	#$1,(g_A6RectRows).w
	bpl.s	loc_0075BE
	move.l	#$75FC,(a6)
	move.l	#$7590,(g_A6ResumeHook).w
; ---------------------------------------------------------------------------
; VM6_BlitJobIdle - V-int list handler of a finished map-blit job: does nothing (an rts) until Frame_End's VM6_EndBlitJob frees the object
; In:  a6 = the job object
; Out: rts
; Evidence: move.l #$75FC,(a6) in VM6_VIntBlitRows after the last row; also its g_FrameBusy exit; 9 corpus runs
; ---------------------------------------------------------------------------
VM6_BlitJobIdle:
	rts
loc_0075FE:
	move.l	d1,(g_A6VdpCmd).w
	rts
; ---------------------------------------------------------------------------
; Op6_DMAEntry - a6 op: DMAs a buffer entry (word byte length, then the data) to VRAM at once, with the display blanked during the transfer unless g_FadeLevel >= $E, then continues
; In:  a6 = script: word g_BufPtrs offset, word entry offset (self-relative long), word VRAM address
; Out: continues; VDP reg 1 back to $64
; Evidence: 46 script uses, e.g. Tbl_01FAB2 `7604 0000 0004 4940`, $1FC28 `7604 0000 0000 7740`; VDP regs $93-$97 + command via g_VDPCmdBuf; 24 corpus runs
; ---------------------------------------------------------------------------
Op6_DMAEntry:
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	adda.w	(a6)+,a0
	adda.l	(a0),a0
	move.w	(a6)+,d0
	move.w	(a0)+,d1
	move.l	a0,d2
	move.w	#$8134,d7
	cmpi.b	#$E,(g_FadeLevel).w
	blt.s	loc_007628
	move.w	#$8174,d7
loc_007628:
	lea.l	($C00004).l,a0
	move.w	d7,(a0)
	lsr.l	#$1,d2
	move.w	d2,d3
	andi.l	#$7F00FF,d2
	ori.l	#$97009500,d2
	lsr.w	#$8,d3
	ori.w	#$9600,d3
	move.l	d2,(a0)
	move.w	d3,(a0)
	lsl.l	#$7,d1
	lsr.w	#$8,d1
	andi.l	#$FF00FF,d1
	ori.l	#$94009300,d1
	move.l	d1,(a0)
	andi.l	#$FFFF,d0
	lsl.l	#$2,d0
	lsr.w	#$2,d0
	swap	d0
	ori.l	#$40000080,d0
	move.l	d0,(g_VDPCmdBuf).w
	move.w	(g_VDPCmdBuf).w,(a0)
	move.w	(g_VDPCmdBuf+2).w,(a0)
	move.w	#$8164,(a0)
	bra.w	VM6_Next
; ---------------------------------------------------------------------------
; Op6_QueueDMAEntry - a6 op: sends a buffer entry (word byte length, then tiles) to VRAM through the frame DMA queue, as many tiles per frame as the DMA budget allows, parking the script until all are sent
; In:  a6 = script: word g_BufPtrs offset, word entry offset (self-relative long), word VRAM address
; Out: parks: g_A6DmaDest, g_A6DmaTiles = length / 32, g_A6DmaSrc, g_A6ResumeHook = VM6_DMAStep
; Evidence: 8 script uses, e.g. $33060 `7682 0000 0000 6340`, $330A0 `7682 0000 0000 6A00` (stage 2 section scripts started by triggers); 8 corpus runs
; ---------------------------------------------------------------------------
Op6_QueueDMAEntry:
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	adda.w	(a6)+,a0
	adda.l	(a0),a0
	move.w	(a6)+,(g_A6DmaDest).w
	move.w	(a0)+,d0
	lsr.w	#$5,d0
	move.w	d0,(g_A6DmaTiles).w
	move.l	a0,(g_A6DmaSrc).w
	move.l	a6,(g_A6ScriptPc).w
	move.l	#$76AE,(g_A6ResumeHook).w
	rts
; ---------------------------------------------------------------------------
; VM6_DMAStep - a6 resume hook of Op6_QueueDMAEntry: queues one DMA of as many tiles as this frame's DMA budget leaves (keeping 5), advancing source and destination; resumes the script when no tiles are left
; In:  g_A6DmaSrc, g_A6DmaDest, g_A6DmaTiles
; Out: one DMA queue entry (g_DMAQueuePtr advanced, g_DMABudgetLeft reduced) or nothing when the budget/queue is short; VM6_Resume when done
; Evidence: hook value $76AE; DMA_Reserve's slot-only entry $79AA with d0 = 1; budget - 5 vs tiles; 8 corpus runs
; ---------------------------------------------------------------------------
VM6_DMAStep:
	moveq	#$0,d2
	move.w	(g_A6DmaTiles).w,d2
	beq.w	VM6_Resume
	move.w	(g_DMABudgetLeft).w,d3
	subq.w	#$5,d3
	ble.s	loc_00772C
	sub.w	d2,d3
	bpl.s	loc_0076C8
	add.w	d3,d2
	moveq	#$0,d3
loc_0076C8:
	moveq	#$1,d0
	jsr	(DMA_ReserveSlots).l
	bne.s	loc_00772C
	move.w	d3,(g_DMABudgetLeft).w
	sub.w	d2,(g_A6DmaTiles).w
	lsl.w	#$5,d2
	move.l	(g_A6DmaSrc).w,d0
	moveq	#$0,d3
	move.w	(g_A6DmaDest).w,d3
	add.l	d2,(g_A6DmaSrc).w
	add.w	d2,(g_A6DmaDest).w
	lsr.l	#$1,d0
	move.w	d0,d1
	andi.l	#$7F00FF,d0
	ori.l	#$97009500,d0
	lsr.w	#$8,d1
	ori.w	#$9600,d1
	move.l	d0,(a0)+
	move.w	d1,(a0)+
	lsl.l	#$7,d2
	lsr.w	#$8,d2
	andi.l	#$FF00FF,d2
	ori.l	#$94009300,d2
	move.l	d2,(a0)+
	lsl.l	#$2,d3
	lsr.w	#$2,d3
	swap	d3
	ori.l	#$40000080,d3
	move.l	d3,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_00772C:
	rts
; ---------------------------------------------------------------------------
; Section_SetIdInline - native called by the a6 op Op6_CallNative: sets g_SectionId to the inline word after the call
; In:  a6 -> section id word
; Out: a6 past it; g_SectionId set
; Evidence: dc.l $772E in 7 a6 scripts, e.g. $3301E `70E0 0000 772E 2200` and $33050 `... 2300` (sections $2200/$2300, set by the stage 2 loader scripts, Tbl_0330FC); 4 corpus runs
; ---------------------------------------------------------------------------
Section_SetIdInline:
	move.w	(a6)+,(g_SectionId).w
	rts
