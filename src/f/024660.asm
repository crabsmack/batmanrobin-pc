; ---------------------------------------------------------------------------
; Stage2_ArenaDrawTilt - draws the arena's tilting floor with scroll alone: plane A line H-scroll and column V-scroll ramps from the slope g_SceneVars+6 about the pivot x $F5A6 and floor y $F5AA, plane B ramps above and below the split line $F5BC that it computes for the V-int
; In:  -
; Out: g_HScrollLinesA/B, g_SceneVars+$A.., $F526.., $F54E.. ramps; $F5BC
; Evidence: Mem_FillRamp with d1 = -slope; tail of Stage2_UpdateScroll in arena mode (TAS f32066-f35416, 3351 frames); png f033600 floor tilted left
; ---------------------------------------------------------------------------
Stage2_ArenaDrawTilt:
	move.w	(g_SceneVars+$6).w,d1
	neg.w	d1
	ext.l	d1
	move.w	(g_SceneVars2+$8).w,d2
	muls.w	d1,d2
	neg.l	d2
	add.l	(g_ShakeY).w,d2
	move.w	(g_SceneVars2+$C).w,d0
	add.w	(g_ShakeY).w,d0
	muls.w	d1,d0
	neg.l	d0
	move.l	d0,d3
	move.l	d1,d4
	move.w	#$E0,d7
	subi.l	#$80000,d0
	lea.l	(g_HScrollLinesA).l,a0
	jsr	(Mem_FillRamp).l
	move.l	(g_SceneVars2+$E).w,d0
	add.l	d2,d0
	asl.l	#$4,d1
	move.l	d1,d5
	move.w	#$14,d7
	lea.l	(g_SceneVars+$A).l,a0
	jsr	(Mem_FillRamp).l
	tst.b	(g_Stage2BossPlaneOn).w
	bne.s	loc_0246C6
	move.w	#$FE00,d7
	clr.w	(g_SceneVars2+$1E).w
	bra.w	loc_02476C
loc_0246C6:
	move.l	d5,d7
	asl.l	#$4,d7
	add.l	d2,d7
	cmp.l	d2,d7
	ble.s	loc_0246D2
	move.l	d2,d7
loc_0246D2:
	swap	d7
	neg.w	d7
	add.w	(g_SceneVars2+$1A).w,d7
	addi.w	#$86,d7
	cmpi.w	#$40,d7
	bge.s	loc_0246E8
	move.w	#$40,d7
loc_0246E8:
	move.w	d7,(g_SceneVars2+$1E).w
	sub.l	(g_SceneVars2+$1A).w,d2
	tst.w	(g_SceneVars+$6).w
	bpl.s	loc_024700
	cmpi.l	#$850000,d2
	ble.s	loc_024736
	bra.s	loc_024708
loc_024700:
	cmpi.l	#$9E0000,d2
	ble.s	loc_024736
loc_024708:
	move.w	#$80,(g_SceneVars2+$1E).w
	moveq	#$0,d0
	lea.l	(g_HScrollLinesB).l,a0
	moveq	#$40,d7
	jsr	(Mem_FillLongs).l
	move.l	#$800080,d0
	lea.l	(g_SceneVars+$32).l,a0
	move.w	#$A,d7
	jsr	(Mem_FillLongs).l
	bra.s	loc_02476A
loc_024736:
	move.l	(g_SceneVars2+$16).w,d0
	add.l	d3,d0
	move.l	d4,d1
	tst.b	(g_Stage2BossPlaneAlt).w
	beq.s	loc_02474A
	addi.l	#$2000000,d0
loc_02474A:
	lea.l	(g_HScrollLinesB).l,a0
	jsr	(Mem_FillRamp).l
	move.l	d2,d0
	move.l	d5,d1
	lea.l	(g_SceneVars+$32).l,a0
	move.w	#$14,d7
	jsr	(Mem_FillRamp).l
loc_02476A:
	moveq	#$0,d7
loc_02476C:
	move.w	(g_SceneVars2+$1E).w,d3
	move.w	(g_SceneVars+$6).w,d1
	ext.l	d1
	move.w	(g_SceneVars2+$8).w,d2
	muls.w	d1,d2
	neg.l	d2
	add.l	(g_ShakeY).w,d2
	move.w	(g_SceneVars2+$C).w,d0
	add.w	(g_ShakeY).w,d0
	muls.w	d1,d0
	neg.l	d0
	addi.l	#$FD500000,d0
	move.w	d3,d4
	muls.w	d1,d4
	add.l	d4,d0
	lea.l	(g_HScrollLinesB).l,a0
	adda.w	d3,a0
	adda.w	d3,a0
	move.w	d7,(a0)+
	move.w	#$DF,d7
	sub.w	d3,d7
	jsr	(Mem_FillRamp).l
	move.l	d2,d0
	asl.l	#$4,d1
	move.w	#$14,d7
	lea.l	(g_SceneVars+$5A).l,a0
	jmp	(Mem_FillRamp).l
; ---------------------------------------------------------------------------
; Coll_Stage2TiltFloor - terrain routine of the stage 2 arena: with floor mask bit 0, lands the mover when its new y reaches the tilted floor y = $C4 - (x - pivot $F5A6) * slope >> 16
; In:  d1/d2 = move, d4/d5 = position, d7 = mask
; Out: d4/d5 moved, d7 = 1 landed / 0, d3 = d6 = 0
; Evidence: installed as g_TerrainFn by Stage2_LiftArriveTop; muls g_SceneVars+6 (the slope drawn by Stage2_ArenaDrawTilt)
; ---------------------------------------------------------------------------
Coll_Stage2TiltFloor:
	moveq	#$0,d3
	moveq	#$0,d6
	add.w	d1,d4
	btst	#$0,d7
	beq.s	loc_0247F2
	move.w	d4,d0
	sub.w	(g_SceneVars2+$8).w,d0
	muls.w	(g_SceneVars+$6).w,d0
	swap	d0
	neg.w	d0
	addi.w	#$C4,d0
	add.w	d2,d5
	cmp.w	d0,d5
	blt.s	loc_0247F4
	move.w	d0,d5
	move.w	#$1,d7
	rts
loc_0247F2:
	add.w	d2,d5
loc_0247F4:
	moveq	#$0,d7
	rts
; ---------------------------------------------------------------------------
; Coll_Stage2TiltFloorRest - resting variant of Coll_Stage2TiltFloor: with mask bit 0 keeps the mover on the tilted floor (y snapped to it, d7 = 1) wherever it moves
; In:  as Coll_Stage2TiltFloor
; Out: d4 moved, d5 = floor y, d7 = 1 (no mask: free move, d7 = 0)
; Evidence: installed as g_TerrainFnAlt by Stage2_LiftArriveTop (bit 0 of (a3): grounded objects)
; ---------------------------------------------------------------------------
Coll_Stage2TiltFloorRest:
	moveq	#$0,d3
	moveq	#$0,d6
	add.w	d1,d4
	btst	#$0,d7
	beq.s	loc_02481E
	move.w	d4,d0
	sub.w	(g_SceneVars2+$8).w,d0
	muls.w	(g_SceneVars+$6).w,d0
	swap	d0
	neg.w	d0
	addi.w	#$C4,d0
	move.w	d0,d5
	move.w	#$1,d7
	rts
loc_02481E:
	add.w	d2,d5
	moveq	#$0,d7
	rts
; ---------------------------------------------------------------------------
; Flight_UpdateCorridor - per-frame scroll inputs of the stage 2 flying section's corridor: the vertical position from g_CameraY, the floor/ceiling stripe phase from g_MapScrollX mod $A000, and 32 middle-wall band offsets advanced by Tbl_024880 rates; then draws it
; In:  -
; Out: g_SceneVars+0/+4, g_SceneVars+$12.. (32 longs) updated; continues in Flight_DrawCorridor
; Evidence: called by $33352 (jmp) and $33448 in the flying scene $33274; g_MapScrollX advances $180 per frame there; png f040200 shows the corridor
; ---------------------------------------------------------------------------
Flight_UpdateCorridor:
	move.l	(g_CameraY).w,d0
	move.l	d0,d1
	lsr.l	#$2,d1
	add.l	d1,d0
	addi.l	#$E80000,d0
	neg.l	d0
	andi.l	#$FFFFFF,d0
	move.l	d0,(g_SceneVars).w
	move.l	(g_MapScrollX).w,d0
	addi.l	#$A000,d0
	divu.w	#$A000,d0
	clr.w	d0
	lsr.l	#$8,d0
	neg.l	d0
	move.l	d0,(g_SceneVars+$4).w
	lea.l	(Tbl_024880).l,a0
	lea.l	(g_SceneVars+$12).l,a1
	move.l	(g_MapScrollX).w,d1
	lsl.l	#$6,d1
	neg.l	d1
	sub.l	(a1),d1
	move.w	#$1F,d7
loc_024872:
	move.l	(a0)+,d0
	add.l	d1,d0
	add.l	d0,(a1)+
	dbra	d7,loc_024872
	bra.w	Flight_DrawCorridor
