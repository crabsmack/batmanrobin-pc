; ---------------------------------------------------------------------------
; Stage3_RevealPlaceTile - a4 native right after the piece script's Op4_Spawn: places the new piece (a5) on its 8x8 cell (cell column/row = the low words of its x/y +$30/+$34; position = cell*8 + camera), gives it the picture's sprite tile (buffer 1 word + $4CA, in +$12), marks the cell taken in g_SceneWorkBuf and queues the plane-B name-table write of the cell (buffer 4 word + $637C) for Stage3_RevealVIntFlush
; In:  a5 = the new piece (a4 object), g_BufPtrs+4 / +$10 = the picture's maps (128 / 40 cells wide)
; Out: $2E/$32/$12(a5), the cell map, g_SceneVars+$A/+$C queue
; Evidence: Op4_CallNative dc.l $03CEF8 in Scr4_03CE3E (+$A, +$76; an a4 script despite its label); VRAM command from g_PlaneBVRAM + (row*128 + col)*2; TAS f97917-f100672 (453 frames)
; ---------------------------------------------------------------------------
Stage3_RevealPlaceTile:
	move.w	$30(a5),d4
	move.w	$34(a5),d5
	move.w	d5,d0
	move.w	d5,d1
	mulu.w	#$28,d1
	asl.w	#$7,d0
	add.w	d4,d0
	add.w	d4,d1
	add.w	d0,d0
	add.w	d1,d1
	asl.w	#$3,d4
	asl.w	#$3,d5
	add.w	(g_CameraX).w,d4
	add.w	(g_CameraY).w,d5
	move.w	d4,$2E(a5)
	move.w	d5,$32(a5)
	lea.l	(g_SceneWorkBuf).l,a0
	move.w	#$1,(a0,d1.w)
	movea.l	(g_BufPtrs+4).w,a0
	move.w	$4(a0,d0.w),d2
	addi.w	#$4CA,d2
	move.w	d2,$12(a5)
	movea.l	(g_BufPtrs+$10).w,a0
	move.w	$4(a0,d1.w),d1
	addi.w	#$637C,d1
	moveq	#$0,d2
	move.w	(g_PlaneBVRAM).w,d2
	add.w	d0,d2
	lsl.l	#$2,d2
	lsr.w	#$2,d2
	ori.w	#$4000,d2
	swap	d2
	lea.l	(g_SceneVars+$C).w,a0
	adda.w	(g_SceneVars+$A).w,a0
	addq.w	#$6,(g_SceneVars+$A).w
	move.l	d2,(a0)+
	move.w	d1,(a0)+
	rts
; ---------------------------------------------------------------------------
; Stage3_RevealDrawTile - a4 native draw op of a reveal piece: puts it straight into the SAT buffer as one 8x8 hardware sprite (tile word $12(a6)) at its screen position; deletes the piece (pops the script return, Op4_Kill) when it is below the screen or the SAT already holds $3C sprites
; In:  a6 = the piece, d4/d5 = its world x/y (from the motion op before it)
; Out: one SAT entry at g_SATPtr, g_SprCount
; Evidence: Op4_CallNative dc.l $03CF72 six times in Scr4_03CE3E; screen = world - camera + $80, limit y < $170; ctarget in 3 corpus runs
; ---------------------------------------------------------------------------
Stage3_RevealDrawTile:
	sub.w	(g_CameraX).w,d4
	sub.w	(g_CameraY).w,d5
	addi.w	#$80,d4
	addi.w	#$80,d5
	cmpi.w	#$170,d5
	bcs.s	loc_03CF90
loc_03CF88:
	addq.l	#$4,a7
	jmp	(Op4_Kill).l
loc_03CF90:
	move.w	(g_SprCount).w,d6
	cmpi.b	#$3C,d6
	bge.s	loc_03CF88
	movea.w	(g_SATPtr).w,a0
	move.w	d5,(a0)+
	addq.b	#$1,d6
	move.w	d6,(a0)+
	move.w	$12(a6),(a0)+
	move.w	d4,(a0)+
	move.w	a0,(g_SATPtr).w
	move.b	d6,(g_SprCountLo).w
	rts
; ---------------------------------------------------------------------------
; Stage3_InitLine3Cycle - sets the start of the clearing's palette-line-3 colour cycle: g_SceneVars2+$22..+$2C = red $A, green $60, blue $E00, steps 2 / $20 / -$200
; In:  -
; Out: g_SceneVars2+$22..+$2D
; Evidence: Op2_CallNativeRel at $03CBB4, first op of the reveal director after its oscillator; ctarget in 1 corpus run (TAS f97888)
; ---------------------------------------------------------------------------
Stage3_InitLine3Cycle:
	move.w	#$A,d0
	move.w	#$60,d1
	move.w	#$E00,d2
	move.w	#$2,d3
	move.w	#$20,d4
	move.w	#$FE00,d5
	movem.w	d0-d5,(g_SceneVars2+$22).w
	rts
; ---------------------------------------------------------------------------
; Stage3_StepLine3Cycle - per-frame colour cycle of palette line 3: advances the walker in g_SceneVars2+$22 by one step (Stage3_Line3CycleStep: blue bounces 0-$E00, at each end green moves, at its ends red moves), then fills colours 1-15 of g_PalSource3 with the next 15 steps and flags the line for reload
; In:  g_SceneVars2+$22..+$2D (Stage3_InitLine3Cycle)
; Out: g_PalSource3+2..+$1F, g_Pal3Reload
; Evidence: Op2_CallNativeRel at $03CBCC in the director's frame loop; ctarget in 5 corpus runs; the pieces and the assembling picture shift colours at f98400/f99600
; ---------------------------------------------------------------------------
Stage3_StepLine3Cycle:
	lea.l	(g_PalSource3+2).w,a0
	movem.w	(g_SceneVars2+$22).w,d0-d5
	bsr.w	Stage3_Line3CycleStep
	movem.w	d0-d5,(g_SceneVars2+$22).w
	moveq	#$E,d7
loc_03CFEA:
	move.w	d0,d6
	or.w	d1,d6
	or.w	d2,d6
	move.w	d6,(a0)+
	bsr.w	Stage3_Line3CycleStep
	dbra	d7,loc_03CFEA
	st.b	(g_Pal3Reload).w
	rts
