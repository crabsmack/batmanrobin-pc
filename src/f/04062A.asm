; ---------------------------------------------------------------------------
; Stage3_RoadCameraFollow - the Hatter road's camera: aims g_CameraX at the players' x (the mean of both when both are in play; x $14A when g_PlayMode bit $800 is set) minus 160, moving at most 3 px per frame and kept within $48-$110, stores -(x * 9/8) as the road's sideways scroll g_SceneVars2+$12, then Camera_UpdateView; with no player in play only Camera_UpdateView
; In:  -
; Out: g_CameraX, g_SceneVars2+$12 (16.16)
; Evidence: Op2_CallNativeRelSaved at $04061E in the road driver Scr2_04061A, every frame; reads g_Player1/2+$2E (x) and g_P1Flags/g_P2Flags bit 0; TAS f113312-f116145
; ---------------------------------------------------------------------------
Stage3_RoadCameraFollow:
	move.w	(g_PlayMode).w,d0
	andi.w	#$800,d0
	beq.w	loc_04063E
	move.l	#$14A0000,d0
	bra.s	loc_04066A
loc_04063E:
	btst	#$0,(g_P1Flags).w
	beq.s	loc_04065E
	move.l	(g_Player1+$2E).w,d0
	btst	#$0,(g_P2Flags).w
	beq.s	loc_04066A
	move.l	(g_Player2+$2E).w,d1
	lsr.l	#$1,d0
	lsr.l	#$1,d1
	add.l	d1,d0
	bra.s	loc_04066A
loc_04065E:
	move.l	(g_Player2+$2E).w,d0
	btst	#$0,(g_P2Flags).w
	beq.s	loc_0406C6
loc_04066A:
	subi.l	#$A00000,d0
	sub.l	(g_CameraX).w,d0
	bpl.s	loc_040686
	cmpi.l	#$FFFD0000,d0
	bge.s	loc_040694
	move.l	#$FFFD0000,d0
	bra.s	loc_040694
loc_040686:
	cmpi.l	#$30000,d0
	ble.s	loc_040694
	move.l	#$30000,d0
loc_040694:
	add.l	(g_CameraX).w,d0
	cmpi.l	#$480000,d0
	bge.s	loc_0406A8
	move.l	#$480000,d0
	bra.s	loc_0406B6
loc_0406A8:
	cmpi.l	#$1100000,d0
	ble.s	loc_0406B6
	move.l	#$1100000,d0
loc_0406B6:
	move.l	d0,(g_CameraX).w
	move.l	d0,d1
	asr.l	#$3,d1
	add.l	d1,d0
	neg.l	d0
	move.l	d0,(g_SceneVars2+$12).w
loc_0406C6:
	jmp	(Camera_UpdateView).l
; ---------------------------------------------------------------------------
; Stage3_DrawRoad - the Hatter road's per-frame update and drawing: clamps the bend g_SceneVars2+$16 (+-24 px) and pitch +$1C, adds the speed (+8, integer part to +$C) to the distance +$10 (+$E = distance mod $80, the checker phase), steps the thrown-toy spin index +$3E (+4 mod $F0), sets the horizon $64 + shake (+$1A) and the screen origin (+$3A/+$3C); then draws the road as plane A with line scroll: for up to 18 checker rows from the bottom line up it writes each row's lines (H-scroll and H-int V-scroll ramps) through Stage3_DrawRoadLinesRun and the row's geometry into g_SceneVars+$12.. (8 bytes per row: line, line, x offset, scale; read by the road objects); the lines above the horizon get Span_ClearLines
; In:  the road state in g_SceneVars2+8..+$3E, set by the boss's road steerer (Boss_Hatter_SteerRoad)
; Out: g_HScrollLinesA and the H-int table g_HIntTblBack (written backwards from line 223), g_SceneVars+$12.. row table, g_SceneVars2 road state
; Evidence: Op2_CallNativeRelSaved at $040622 in Scr2_04061A, every frame; depth tables Tbl_040C86/Tbl_040ECA; 18 rows = d7 $11; TAS f113312-f116145; png f113400-f115800: the checkered road bends and dips
; ---------------------------------------------------------------------------
Stage3_DrawRoad:
	move.l	(g_SceneVars2+$1C).w,d0
	cmpi.l	#$FE200000,d0
	bge.s	loc_0406E0
	move.l	#$FE200000,d0
	bra.s	loc_0406EE
loc_0406E0:
	cmpi.l	#$280000,d0
	ble.s	loc_0406EE
	move.l	#$280000,d0
loc_0406EE:
	move.l	d0,(g_SceneVars2+$1C).w
	move.l	(g_SceneVars2+$16).w,d0
	cmpi.l	#$FFE80000,d0
	bge.s	loc_040706
	move.l	#$FFE80000,d0
	bra.s	loc_040714
loc_040706:
	cmpi.l	#$180000,d0
	ble.s	loc_040714
	move.l	#$180000,d0
loc_040714:
	move.l	d0,(g_SceneVars2+$16).w
	move.w	(g_SceneVars2+$8).w,d0
	move.w	d0,(g_SceneVars2+$C).w
	add.w	(g_SceneVars2+$10).w,d0
	move.w	d0,(g_SceneVars2+$10).w
	andi.w	#$7F,d0
	move.w	d0,(g_SceneVars2+$E).w
	move.w	#$64,d0
	add.w	(g_ShakeY).w,d0
	move.w	d0,(g_SceneVars2+$1A).w
	move.w	(g_SceneVars2+$3E).w,d0
	addq.w	#$4,d0
	cmpi.w	#$F0,d0
	blt.s	loc_04074C
	subi.w	#$F0,d0
loc_04074C:
	move.w	d0,(g_SceneVars2+$3E).w
	move.w	#$C0,d0
	sub.w	(g_SprOffsetX).w,d0
	move.w	d0,(g_SceneVars2+$3A).w
	move.w	#$1F,d0
	sub.w	(g_SprOffsetY).w,d0
	move.w	d0,(g_SceneVars2+$3C).w
	bra.w	loc_04076C
loc_04076C:
	lea.l	(g_HScrollLinesB).l,a0
	movea.w	(g_HIntTblBack).w,a1
	lea.l	$1C0(a1),a1
	lea.l	(Tbl_040C86-$20).l,a2	; row depth table: (a2) and -$20(a2) per row, +$20 each row
	lea.l	(Tbl_040ECA).l,a3
	lea.l	(g_SceneVars+$12).l,a4
	move.w	#$1000,(g_SceneVars2+$24).w
	moveq	#$0,d0
	move.w	(g_SceneVars2+$1A).w,d0
	move.w	d0,(g_SceneVars2+$22).w
	swap	d0
	asl.l	#$3,d0
	move.l	d0,(g_SceneVars2+$2A).w
	move.l	(g_SceneVars2+$12).w,d0
	move.l	d0,(g_SceneVars2+$32).w
	asl.l	#$6,d0
	swap	d0
	ext.l	d0
	move.l	d0,(g_SceneVars2+$36).w
	move.w	(g_SceneVars2+$10).w,d0
	andi.w	#$80,d0
	add.w	d0,d0
	move.w	d0,(g_SceneVars2+$20).w
	move.w	(g_SceneVars2+$E).w,d0
	andi.w	#$78,d0
	move.w	d0,d1
	lsr.w	#$3,d0
	neg.w	d0
	addi.w	#$28,d0
	add.w	d0,d0
	adda.w	d0,a2
	move.l	(g_SceneVars2+$1C).w,d0
	asl.l	#$6,d0
	swap	d0
	muls.w	d1,d0
	asl.l	#$3,d0
	neg.l	d0
	move.l	d0,(g_SceneVars2+$26).w
	move.l	(g_SceneVars2+$16).w,d0
	asr.l	#$7,d0
	muls.w	d1,d0
	neg.l	d0
	move.l	d0,(g_SceneVars2+$2E).w
	move.w	-$20(a2),d0
	move.w	(g_SceneVars2+$2A).w,d1
	subi.w	#$240,d1
	muls.w	d0,d1
	add.l	d1,d1
	swap	d1
	addi.w	#$48,d1
	addi.w	#$DF,d1
	sub.w	(g_SceneVars2+$1A).w,d1
	move.w	d1,(a4)+
	move.w	d1,(a4)+
	move.w	(g_SceneVars2+$32).w,(a4)+
	move.w	d0,(a4)+
	move.l	#$E000E0,d6
	move.w	#$11,d7
	bra.w	Stage3_DrawRoadRow
; ---------------------------------------------------------------------------
; Stage3_DrawRoadNextRow - Stage3_DrawRoad's row loop head: toggles the checker colour, stores the row just drawn (its two lines, x offset, depth), then steps the bend and pitch slopes to the next row (or, after the last row, sets up the horizon's flat stretch from Tbl_040EB6)
; In:  the row loop's registers (a2 depth table, a4 row table, d6 lines, d7 rows left)
; Out: g_SceneVars2+$20/+$26/+$2A/+$2E/+$32, a4 past the row; continues at Stage3_DrawRoadRow
; Evidence: dbra target of Stage3_DrawRoadLoopRows ($040C3E) and Stage3_DrawRoad's own dbra; eori.w #$100 / move.l d6,(a4)+ / tst.w d7; TAS f113312-f116145
; ---------------------------------------------------------------------------
Stage3_DrawRoadNextRow:
	eori.w	#$100,(g_SceneVars2+$20).w
	move.l	d6,(a4)+
	move.w	(g_SceneVars2+$32).w,(a4)+
	move.w	(a2),(a4)+
	tst.w	d7
	bne.s	loc_04089E
	lea.l	(Tbl_040EB6).l,a2
	move.l	(g_SceneVars2+$16).w,d0
	asr.l	#$6,d0
	muls.w	#$2200,d0
	add.l	(g_SceneVars2+$12).w,d0
	move.l	d0,(g_SceneVars2+$32).w
	move.l	(g_SceneVars2+$1C).w,d0
	bpl.s	loc_040878
	neg.l	d0
	move.l	d0,d1
	swap	d0
	mulu.w	#$31C,d0
	mulu.w	#$31C,d1
	swap	d0
	clr.w	d0
	add.l	d1,d0
	neg.l	d0
	bra.s	loc_04088A
loc_040878:
	move.l	d0,d1
	swap	d0
	mulu.w	#$31C,d0
	mulu.w	#$31C,d1
	swap	d0
	clr.w	d0
	add.l	d1,d0
loc_04088A:
	moveq	#$0,d1
	move.w	(g_SceneVars2+$1A).w,d1
	ext.l	d1
	asl.l	#$3,d1
	swap	d1
	add.l	d1,d0
	move.l	d0,(g_SceneVars2+$2A).w
	bra.s	Stage3_DrawRoadRow
loc_04089E:
	lea.l	$20(a2),a2
	move.l	(g_SceneVars2+$16).w,d0
	add.l	(g_SceneVars2+$2E).w,d0
	move.l	d0,(g_SceneVars2+$2E).w
	add.l	d0,(g_SceneVars2+$32).w
	move.l	(g_SceneVars2+$26).w,d0
	add.l	(g_SceneVars2+$1C).w,d0
	move.l	d0,d1
	asr.l	#$2,d1
	add.l	d1,d0
	move.l	d0,(g_SceneVars2+$26).w
	add.l	d0,(g_SceneVars2+$2A).w
; ---------------------------------------------------------------------------
; Stage3_DrawRoadRow - one road row: its depth from the table, the x offset and height scaled by it, the row's line count (the height step); a row with lines continues in Stage3_DrawRoadRowLines, an empty or hidden one loops to the next row
; In:  the row loop's registers
; Out: g_SceneVars2+$22/+$24/+$36; d0-d5 the row's scroll ramps
; Evidence: bra.w from Stage3_DrawRoad's setup ($04082C) and Stage3_DrawRoadNextRow's last-row path; ble.w Stage3_DrawRoadLoopRows / bpl.w / bgt.w Stage3_DrawRoadRowLines; TAS f113312-f116145
; ---------------------------------------------------------------------------
Stage3_DrawRoadRow:
	move.w	(a2),d3
	move.l	(g_SceneVars2+$36).w,d0
	move.w	(g_SceneVars2+$32).w,d1
	muls.w	d3,d1
	asr.l	#$6,d1
	move.l	d1,(g_SceneVars2+$36).w
	move.w	(g_SceneVars2+$24).w,d2
	move.w	d3,(g_SceneVars2+$24).w
	move.w	(g_SceneVars2+$2A).w,d4
	muls.w	d3,d4
	add.l	d4,d4
	swap	d4
	sub.w	(g_SceneVars2+$22).w,d4
	add.w	d4,(g_SceneVars2+$22).w
	swap	d6
	add.w	d4,d6
	swap	d6
	neg.w	d4
	ble.w	Stage3_DrawRoadLoopRows
	move.w	d4,d5
	add.w	d5,d5
	move.w	(a3,d5.w),d5
	sub.l	d0,d1
	muls.w	d5,d1
	ror.l	#$6,d0
	addi.w	#$A0,d0
	sub.w	d2,d3
	muls.w	d5,d3
	asl.l	#$2,d3
	addi.l	#$10000,d3
	ext.l	d2
	ror.l	#$4,d2
	sub.w	(g_SceneVars2+$20).w,d2
	sub.w	d6,d2
	sub.w	d4,d6
	bpl.w	Stage3_DrawRoadRowLines
	add.w	d6,d4
	bgt.w	Stage3_DrawRoadRowLines
	ext.l	d6
	dbra	d7,Stage3_DrawRoadNextRow
	move.l	d6,(a4)+
	move.w	(g_SceneVars2+$32).w,(a4)+
	move.w	(a2),(a4)+
; ---------------------------------------------------------------------------
; Stage3_DrawRoadDone - Stage3_DrawRoad's return when the rows reach the top line (no lines above the road to clear)
; In:  -
; Out: -
; Evidence: rts at $040942; ble.w from $040C52 after the last row (d6 <= 0)
; ---------------------------------------------------------------------------
Stage3_DrawRoadDone:
	rts
