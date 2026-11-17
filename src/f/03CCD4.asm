; ---------------------------------------------------------------------------
; Stage3_RevealVIntFlush - V-int list object of the reveal: writes the queued plane-B cells (g_SceneVars+$C: entries of a VDP command long + name-table word, g_SceneVars+$A bytes) to the VDP and empties the queue
; In:  a0/a1 = VDP control/data ports (V-int), g_SceneVars+$A/+$C queue
; Out: g_SceneVars+$A = 0
; Evidence: made by Stage3_RevealInit (VIntObj_New); entries queued by Stage3_RevealPlaceTile; TAS f97888-f101674 (3787 frames)
; ---------------------------------------------------------------------------
Stage3_RevealVIntFlush:
	move.w	(g_SceneVars+$A).w,d7
	beq.s	loc_03CCEA
	lea.l	(g_SceneVars+$C).w,a4
loc_03CCDE:
	move.l	(a4)+,(a0)
	move.w	(a4)+,(a1)
	subq.w	#$6,d7
	bgt.s	loc_03CCDE
	clr.w	(g_SceneVars+$A).w
loc_03CCEA:
	rts
; ---------------------------------------------------------------------------
; Stage3_RevealBurst - one reveal burst: shortens the burst interval (timer 0 = timer 1 -= timer 2), picks the next of 36 spots (9x4, order from the seed g_SceneVars+6: x*$555+1 mod 64, skipping >= 36), launches ToyFigure_SpawnFlyer there and reveals a random-walk patch of up to 25 cells around it (Fx_RunScript Scr4_03CE3E per cell)
; In:  a4 = the director's a4 block (timers +$1A/$1C/$1E)
; Out: g_SceneVars+6; pieces spawned
; Evidence: Op2_CallNativeRelSaved at $03CBEE, $24 times (Op2_LoopCounter0) after sound $11; cell = spot*4 + rand 0-3 / spot row*4 + rand 0-1, pixel = cell*8 + $14 + camera; ctarget in 3 corpus runs (TAS f97917-f100195 by ToyFigure_SpawnFlyer's 36 frames)
; ---------------------------------------------------------------------------
Stage3_RevealBurst:
	move.w	$1C(a4),d0
	sub.w	$1E(a4),d0
	move.w	d0,$1A(a4)
	move.w	d0,$1C(a4)
	move.w	(g_SceneVars+$6).w,d0
loc_03CD00:
	mulu.w	#$555,d0
	addq.w	#$1,d0
	andi.l	#$3F,d0
	cmpi.w	#$24,d0
	bge.s	loc_03CD00
	move.w	d0,(g_SceneVars+$6).w
	divu.w	#$9,d0
	move.w	d0,d5
	swap	d0
	move.w	d0,d4
	asl.w	#$2,d4
	asl.w	#$2,d5
	jsr	(Rand_Next).l
	andi.w	#$3,d0
	add.w	d0,d4
	jsr	(Rand_Next).l
	andi.w	#$1,d0
	add.w	d0,d5
	movem.w	d4-d5,-(a7)
	asl.w	#$3,d4
	asl.w	#$3,d5
	addi.w	#$14,d4
	addi.w	#$14,d5
	add.w	(g_CameraX).w,d4
	add.w	(g_CameraY).w,d5
	jsr	(ToyFigure_SpawnFlyer).l
	movem.w	(a7)+,d4-d5
	moveq	#$0,d6
	moveq	#$0,d7
loc_03CD62:
	jsr	(Rand_Next).l
	andi.w	#$1,d0
	add.w	d0,d6
	cmpi.w	#$5,d6
	blt.s	loc_03CD80
	subq.w	#$5,d6
	addq.w	#$1,d7
	cmpi.w	#$5,d7
	blt.s	loc_03CD80
	rts
loc_03CD80:
	movem.w	d4-d7,-(a7)
	add.w	d6,d4
	add.w	d7,d5
	lea.l	(Scr4_03CE3E).l,a0
	jsr	(Fx_RunScript).l
	movem.w	(a7)+,d4-d7
	addq.w	#$1,d6
	bra.s	loc_03CD62
; ---------------------------------------------------------------------------
; Stage3_RevealNextCell - reveals the next cell not yet taken: scans the cell map on from the cursor (g_SceneVars+0/+2/+4, 40 columns x 22 rows) and runs the piece script Scr4_03CE3E there; d0 = 1 once the whole map is done
; In:  g_SceneVars+0 (map pointer), +2/+4 (column, row)
; Out: d0 = 0 piece spawned / 1 finished; cursor updated
; Evidence: Op2_CallNativeRelSwitch at $03CC56 (the director's fill loop, with random white flashes, sounds $37/$39/$3A/$3D and shakes); ctarget in 3 corpus runs
; ---------------------------------------------------------------------------
Stage3_RevealNextCell:
	move.w	(g_SceneVars+$2).w,d4
	move.w	(g_SceneVars+$4).w,d5
	movea.w	(g_SceneVars).w,a0
loc_03CDA8:
	addq.w	#$1,d4
	cmpi.w	#$28,d4
	blt.s	loc_03CDBE
	moveq	#$0,d4
	addq.w	#$1,d5
	cmpi.w	#$16,d5
	blt.s	loc_03CDBE
	moveq	#$1,d0
	rts
loc_03CDBE:
	tst.w	(a0)+
	bne.s	loc_03CDA8
	move.w	a0,(g_SceneVars).w
	move.w	d4,(g_SceneVars+$2).w
	move.w	d5,(g_SceneVars+$4).w
	lea.l	(Scr4_03CE3E).l,a0
	jsr	(Fx_RunScript).l
	moveq	#$0,d0
	rts
