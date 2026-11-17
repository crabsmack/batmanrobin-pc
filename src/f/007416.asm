; ---------------------------------------------------------------------------
; VM6_StartRedraw - a6: starts a column-per-frame redraw of a layer: saves its block column +$26 and plane column +$32, takes the column count g_ScreenBlocksX from +$3E and installs VM6_RedrawStep
; In:  a0 = layer, a6 = script pc to resume at
; Out: parks: g_A6RedrawLayer = a0, g_A6RedrawCols = +$3E, g_A6RedrawSavedCol/Cell = +$26/+$32, g_A6ResumeHook = $743A
; Evidence: bra from Op6_RedrawLayerA (and the raw $73FE); 4 corpus runs (TAS f67239)
; ---------------------------------------------------------------------------
VM6_StartRedraw:
	move.l	a6,(g_A6ScriptPc).w
	move.w	$26(a0),(g_A6RedrawSavedCol).w
	move.w	$32(a0),(g_A6RedrawSavedCell).w
	move.w	$3E(a0),(g_A6RedrawCols).w
	move.l	a0,(g_A6RedrawLayer).w
	move.l	#$743A,(g_A6ResumeHook).w
	rts
; ---------------------------------------------------------------------------
; VM6_RedrawStep - a6 resume hook of the layer redraw: queues the next block column of the layer (Layer_QueueColumn, step 1); after the last one restores the layer's column counters and resumes the script on the next frame
; In:  g_A6RedrawLayer, g_A6RedrawCols
; Out: rts; when done +$26/+$32 restored and g_A6ResumeHook = VM6_Resume
; Evidence: hook value $743A; Layer_QueueColumn d0 = 1, d1 = 0, d2 = 2, d3 = 0 (one block column = 2 plane cells); 4 corpus runs
; ---------------------------------------------------------------------------
VM6_RedrawStep:
	movea.l	(g_A6RedrawLayer).w,a6
	moveq	#$1,d0
	moveq	#$0,d1
	moveq	#$2,d2
	moveq	#$0,d3
	jsr	(Layer_QueueColumn).l
	subq.w	#$1,(g_A6RedrawCols).w
	bgt.s	loc_007466
	move.w	(g_A6RedrawSavedCol).w,$26(a6)
	move.w	(g_A6RedrawSavedCell).w,$32(a6)
	move.l	#$70A8,(g_A6ResumeHook).w
loc_007466:
	rts
; ---------------------------------------------------------------------------
; Op6_DrawMap - a6 op: writes a whole cell map from a buffer entry onto a plane straight to the VDP (scene setup), then continues
; In:  a6 = script: word g_BufPtrs offset, word entry offset, word address of the plane VRAM variable, word tile word added to every cell
; Out: continues (VM6_BlitMapNow)
; Evidence: d0-d5 = 0 (whole map at the plane origin) / bsr VM6_SetupMapBlit / bra VM6_BlitMapNow; 34 script uses, e.g. $1FC16 `7468 0000 0000 EE36 02D2` (plane B, tile + $2D2); 24 corpus runs
; ---------------------------------------------------------------------------
Op6_DrawMap:
	moveq	#$0,d0
	moveq	#$0,d1
	moveq	#$0,d2
	moveq	#$0,d3
	moveq	#$0,d4
	moveq	#$0,d5
	bsr.w	VM6_SetupMapBlit
	bra.w	VM6_BlitMapNow
; ---------------------------------------------------------------------------
; Op6_DrawMapRect - a6 op: writes a rectangle of a cell map from a buffer entry onto a plane position straight to the VDP, then continues
; In:  a6 = script: words src x (bytes), src row, dest x (bytes), dest row, width, height (cells, 0 = the map's), then as Op6_DrawMap
; Out: continues (VM6_BlitMapNow)
; Evidence: movem.w (a6)+,d0-d5; 10 script uses, e.g. $32E74 `747C 0000 000C 0058 0016 0028 0008 0000 0000 EE34 0396` (40 x 8 cells from row 12 to plane A row 22); 9 corpus runs
; ---------------------------------------------------------------------------
Op6_DrawMapRect:
	movem.w	(a6)+,d0-d5
	bsr.w	VM6_SetupMapBlit
	bra.w	VM6_BlitMapNow
; ---------------------------------------------------------------------------
; Op6_DrawMapVInt - a6 op: writes a whole cell map from a buffer entry onto a plane through a V-int job (rows as the DMA budget allows), parking the script until done
; In:  a6 = script: word g_BufPtrs offset, word entry offset, word address of the plane VRAM variable, word tile word
; Out: parks (VM6_StartBlitJob)
; Evidence: d0-d5 = 0 / bsr VM6_SetupMapBlit / bra VM6_StartBlitJob; 2 script uses: $33114 `7488 0004 0000 EE36 455A`, $49D9A `7488 0000 0000 EE36 02B6`; 8 corpus runs
; ---------------------------------------------------------------------------
Op6_DrawMapVInt:
	moveq	#$0,d0
	moveq	#$0,d1
	moveq	#$0,d2
	moveq	#$0,d3
	moveq	#$0,d4
	moveq	#$0,d5
	bsr.w	VM6_SetupMapBlit
	bra.w	VM6_StartBlitJob
; ---------------------------------------------------------------------------
; Op6_DrawMapRectVInt - a6 op: writes a rectangle of a cell map from a buffer entry onto a plane position through a V-int job, parking the script until done
; In:  a6 = script: words src x (bytes), src row, dest x (bytes), dest row, width, height, then as Op6_DrawMapVInt
; Out: parks (VM6_StartBlitJob)
; Evidence: movem.w (a6)+,d0-d5 / bsr VM6_SetupMapBlit / bra VM6_StartBlitJob; 6 corpus runs
; ---------------------------------------------------------------------------
Op6_DrawMapRectVInt:
	movem.w	(a6)+,d0-d5
	bsr.w	VM6_SetupMapBlit
	bra.w	VM6_StartBlitJob
