; ---------------------------------------------------------------------------
; Boss_CatHead_StartSwayRaster - switches the cat-head arena to the sway raster: g_SceneVars+0 = Boss_CatHead_SwayRaster, scroll mode 4 with g_ScrollVIntFn = Boss_CatHead_SwayVInt, pivot g_SceneVars+$12/$14 = ($A0, $68)
; In:  -
; Out: g_SceneVars+0, +$12, +$14, g_ScrollMode, g_ScrollVIntFn
; Evidence: Op2_CallNativeRelSaved at $03BA1A (the driver after the curtain has risen); TAS f87704 and f90023 (2 frames)
; ---------------------------------------------------------------------------
Boss_CatHead_StartSwayRaster:
	move.l	#Boss_CatHead_SwayRaster,(g_SceneVars).w
	move.w	#$4,(g_ScrollMode).w
	move.l	#Boss_CatHead_SwayVInt,(g_ScrollVIntFn).w
	move.w	#$A0,(g_SceneVars+$12).w
	move.w	#$68,(g_SceneVars+$14).w
	rts
; ---------------------------------------------------------------------------
; Boss_CatHead_SwayVInt - one-shot V-int switch into the sway scroll: g_ScrollMode = $2C, VDP reg $0B = $07 (line H-scroll, 2-cell column V-scroll), then the standard mode handler
; In:  a0 = VDP control port (V-int)
; Out: jmp VInt_ScrollLinesColumns
; Evidence: stored in g_ScrollVIntFn by Boss_CatHead_StartSwayRaster and reached through scroll mode 4 (VInt_ScrollCustom); ctarget in 4 corpus runs
; ---------------------------------------------------------------------------
Boss_CatHead_SwayVInt:
	move.w	#$2C,(g_ScrollMode).w
	move.w	#$8B07,(a0)
	jmp	(VInt_ScrollLinesColumns).l
; ---------------------------------------------------------------------------
; Boss_CatHead_SwayRaster - per-frame sway/tilt of the face picture: plane A line H-scroll = a ramp of slope -(g_SceneVars+$10) about the pivot line +$14, its 20 column V-scrolls a ramp of the same slope about the pivot x +$12 (a small rotation), plus a bulge from +$C added symmetrically from the centre column out (1.5x per column); plane B columns = g_ViewY
; In:  g_SceneVars+$C, +$10, +$12, +$14 (Boss_CatHead_DriverSway, StartSwayRaster)
; Out: g_HScrollLinesA, g_SceneWorkBuf (plane A column V-scroll), g_SceneWorkBuf+$28 (plane B columns)
; Evidence: called through g_SceneVars+0 by Boss_CatHead_FrameScroll; Mem_FillRamp x2 + loop + Mem_FillWords; ctarget in 6 corpus runs; the tilted face at f88000
; ---------------------------------------------------------------------------
Boss_CatHead_SwayRaster:
	move.w	(g_SceneVars+$10).w,d1
	neg.w	d1
	ext.l	d1
	move.w	(g_SceneVars+$12).w,d2
	muls.w	d1,d2
	neg.l	d2
	move.w	(g_SceneVars+$14).w,d0
	muls.w	d1,d0
	neg.l	d0
	move.w	#$E0,d7
	lea.l	(g_HScrollLinesA).l,a0
	jsr	(Mem_FillRamp).l
	moveq	#$0,d0
	move.w	(g_VScrollA).w,d0
	swap	d0
	add.l	d2,d0
	asl.l	#$4,d1
	move.l	d1,d5
	move.w	#$14,d7
	lea.l	(g_SceneWorkBuf).l,a0
	jsr	(Mem_FillRamp).l
	moveq	#$0,d0
	move.l	(g_SceneVars+$C).w,d1
	lea.l	(g_SceneWorkBuf+$14).l,a0
	movea.l	a0,a1
	moveq	#$8,d7
loc_03BC12:
	add.l	d1,d0
	swap	d0
	add.w	d0,(a0)+
	add.w	d0,-(a1)
	swap	d0
	move.l	d1,d2
	asr.l	#$1,d2
	add.l	d2,d1
	dbra	d7,loc_03BC12
	move.w	#$14,d7
	move.w	(g_ViewY).w,d0
	lea.l	(g_SceneWorkBuf+$28).l,a0
	jmp	(Mem_FillWords).l
; ---------------------------------------------------------------------------
; Boss_CatHead_StartCurtainRaster - switches the cat-head arena to the curtain raster: g_SceneVars+0 = Boss_CatHead_CurtainRaster, scroll mode 4 with g_ScrollVIntFn = Boss_CatHead_CurtainVInt, and installs the per-line V-scroll H-int on plane B
; In:  -
; Out: g_SceneVars+0, g_ScrollMode, g_ScrollVIntFn; jmp HInt_InstallVScrollB
; Evidence: Op2_CallNativeRelSaved at $03B962 (the driver's curtain coroutine); ctarget in 2 corpus runs; the red curtain (plane B) over the arena at f87400/f89640
; ---------------------------------------------------------------------------
Boss_CatHead_StartCurtainRaster:
	move.l	#Boss_CatHead_CurtainRaster,(g_SceneVars).w
	move.w	#$4,(g_ScrollMode).w
	move.l	#Boss_CatHead_CurtainVInt,(g_ScrollVIntFn).w
	jmp	(HInt_InstallVScrollB).l
; ---------------------------------------------------------------------------
; Boss_CatHead_CurtainVInt - one-shot V-int switch into the curtain scroll: g_ScrollMode = $3C (planes + H-int raster), VDP reg $0B = $03, then the standard mode handler
; In:  a0 = VDP control port (V-int)
; Out: jmp VInt_ScrollPlanesRaster
; Evidence: stored in g_ScrollVIntFn by Boss_CatHead_StartCurtainRaster, reached through scroll mode 4; ctarget in 2 corpus runs
; ---------------------------------------------------------------------------
Boss_CatHead_CurtainVInt:
	move.w	#$3C,(g_ScrollMode).w
	move.w	#$8B03,(a0)
	jmp	(VInt_ScrollPlanesRaster).l
; ---------------------------------------------------------------------------
; Boss_CatHead_CurtainRaster - per-frame curtain raster: squeezes the $C0-line curtain picture of plane B into the top h lines (h = g_SceneVars+4 clamped to 1-$C0, less g_ShakeY) with a line V-scroll ramp, the lines below show the plane at g_ViewY
; In:  g_SceneVars+4 (Boss_CatHead_DriverCurtainY), g_ShakeY, g_ViewY
; Out: the H-int line table g_HIntTblBack
; Evidence: called through g_SceneVars+0 by Boss_CatHead_FrameScroll; divu $C000/h, Mem_FillRamp + Mem_FillWords; ctarget in 4 corpus runs; shots f87400 (curtain low) / f87560 (raised) / f89640 (closed) / f89880
; ---------------------------------------------------------------------------
Boss_CatHead_CurtainRaster:
	move.w	(g_SceneVars+$4).w,d7
	bgt.s	loc_03BC6E
	moveq	#$1,d7
loc_03BC6E:
	cmpi.w	#$C0,d7
	bcs.s	loc_03BC78
	move.w	#$C0,d7
loc_03BC78:
	move.w	#$C0,d0
	move.l	#$C000,d1
	sub.w	(g_ShakeY).w,d7
	beq.s	loc_03BC8A
	divu.w	d7,d1
loc_03BC8A:
	sub.w	d7,d0
	mulu.w	d1,d0
	subi.w	#$100,d1
	swap	d1
	clr.w	d1
	lsr.l	#$8,d1
	asl.l	#$6,d0
	addi.l	#$F00000,d0
	movea.w	(g_HIntTblBack).w,a0
	move.w	#$C0,d6
	sub.w	d7,d6
	lsr.w	#$2,d6
	sub.w	d6,d7
	ble.s	loc_03BCC2
	move.w	d7,d6
	jsr	(Mem_FillRamp).l
	move.w	#$E0,d7
	sub.w	d6,d7
	bgt.s	loc_03BCC6
	rts
loc_03BCC2:
	move.w	#$E0,d7
loc_03BCC6:
	move.w	(g_ViewY).w,d0
	jmp	(Mem_FillWords).l
; ---------------------------------------------------------------------------
; Boss_CatHead_StartFallRaster - switches the cat-head arena to the fall raster after the defeat: g_SceneVars+0 = Boss_CatHead_FallRaster, scroll mode 4 with g_ScrollVIntFn = Boss_CatHead_FallVInt, and installs the per-line V-scroll H-int on plane A
; In:  -
; Out: g_SceneVars+0, g_ScrollMode, g_ScrollVIntFn; jmp HInt_InstallVScrollA
; Evidence: Op2_CallNativeRelSaved at $03BB08 (Scr2_03BB06, the driver's last coroutine, which ends in Section_StartExit); ctarget in 3 corpus runs
; ---------------------------------------------------------------------------
Boss_CatHead_StartFallRaster:
	move.l	#Boss_CatHead_FallRaster,(g_SceneVars).w
	move.w	#$4,(g_ScrollMode).w
	move.l	#Boss_CatHead_FallVInt,(g_ScrollVIntFn).w
	jmp	(HInt_InstallVScrollA).l
; ---------------------------------------------------------------------------
; Boss_CatHead_FallVInt - one-shot V-int switch into the fall scroll: g_ScrollMode = $3C, VDP reg $0B = $03, then the standard mode handler
; In:  a0 = VDP control port (V-int)
; Out: jmp VInt_ScrollPlanesRaster
; Evidence: stored in g_ScrollVIntFn by Boss_CatHead_StartFallRaster, reached through scroll mode 4; ctarget in 3 corpus runs
; ---------------------------------------------------------------------------
Boss_CatHead_FallVInt:
	move.w	#$3C,(g_ScrollMode).w
	move.w	#$8B03,(a0)
	jmp	(VInt_ScrollPlanesRaster).l
; ---------------------------------------------------------------------------
; Boss_CatHead_FallRaster - per-frame raster of the falling face plate: plane B V-scroll = g_ViewY; palette line 3 tint -2 while g_SceneVars+6 < 0; plane A's line table shows nothing above the plate, the plate scaled into the lines above the floor line $AA (+6 = $AA upright, smaller = tipping over) and the floor below
; In:  g_SceneVars+6 (Boss_CatHead_DriverFallY), g_VScrollA, g_ViewY
; Out: g_VScrollB, g_Pal3TintR/G/B, the H-int line table g_HIntTblBack
; Evidence: called through g_SceneVars+0 by Boss_CatHead_FrameScroll; divs $AA00/+6, Mem_FillRamp + Mem_FillWords; ctarget in 3 corpus runs; shots f90150/f90300 (the plate flat on the floor)
; ---------------------------------------------------------------------------
Boss_CatHead_FallRaster:
	move.w	(g_ViewY).w,d0
	move.w	d0,(g_VScrollB).w
	moveq	#$0,d0
	move.w	(g_SceneVars+$6).w,d7
	bpl.s	loc_03BD0E
	moveq	#-$2,d0
loc_03BD0E:
	move.b	d0,(g_Pal3TintR).l
	move.b	d0,(g_Pal3TintG).l
	move.b	d0,(g_Pal3TintB).l
	movea.w	(g_HIntTblBack).w,a0
	move.w	#$E0,d6
	move.w	#$AA,d7
	move.w	(g_SceneVars+$6).w,d0
	bmi.s	loc_03BD34
	sub.w	d0,d7
loc_03BD34:
	sub.w	d7,d6
	moveq	#-$1,d0
	bra.s	loc_03BD3E
loc_03BD3A:
	move.w	d0,(a0)+
	subq.w	#$1,d0
loc_03BD3E:
	dbra	d7,loc_03BD3A
	swap	d0
	clr.w	d0
	move.l	#$AA00,d1
	move.w	(g_SceneVars+$6).w,d2
	bge.s	loc_03BD78
	neg.w	d2
	divs.w	d2,d1
	ext.l	d1
	asl.l	#$8,d1
	subi.l	#$10000,d1
	addi.l	#$BE0000,d0
	neg.l	d1
	move.w	(g_SceneVars+$6).w,d7
	neg.w	d7
	muls.w	#$11E,d7
	asr.l	#$7,d7
	bra.w	loc_03BD9E
loc_03BD78:
	bne.s	loc_03BD7C
	moveq	#$1,d2
loc_03BD7C:
	divs.w	d2,d1
	move.w	d1,d7
	ext.l	d1
	asl.l	#$8,d1
	subi.l	#$10000,d1
	swap	d0
	add.w	(g_VScrollA).w,d0
	addq.w	#$1,d0
	swap	d0
	move.w	(g_SceneVars+$6).w,d7
	muls.w	#$11E,d7
	asr.l	#$8,d7
loc_03BD9E:
	sub.w	d7,d6
	bpl.s	loc_03BDA4
	add.w	d6,d7
loc_03BDA4:
	jsr	(Mem_FillRamp).l
	move.w	d6,d7
	ble.s	loc_03BDB8
	move.w	#$E0,d0
	jmp	(Mem_FillWords).l
loc_03BDB8:
	rts
; ---------------------------------------------------------------------------
; Boss_CatHead_Spawn - trigger callback that spawns the cat-head boss of the stage 3 backlot arena (Scr2_03BDC6: attack-schedule coroutine 0 on vector 0, then the head body at $03B21C that spawns both paws and the raster driver)
; In:  d4/d5 = position (trigger entry)
; Out: jumps to Obj_SpawnBoss with a2 = Scr2_03BDC6
; Evidence: the only entry of Tbl_033F20 (dc.l $03BDBA), loaded by the arena scene $03FFBE; Scr2_03BDC6 Op2_Goto $03B21C; ctarget in 2 corpus runs; the head is the object at (160,99) in RAM f88320, the grinning cat face on plane A at f87600-f90000
; ---------------------------------------------------------------------------
Boss_CatHead_Spawn:
	lea.l	(Scr2_03BDC6).l,a2
	jmp	(Obj_SpawnBoss).l
