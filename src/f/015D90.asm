; ---------------------------------------------------------------------------
; Stage1_ScrollFacadeLines - fills the plane B line scroll of the stage 1 street in five bands whose shear depends on the offset in the facade segment, so the building's parts scroll as if at different depths
; In:  $F50A = offset in the facade segment (16.16), g_ViewY
; Out: g_HScrollLinesB rows set
; Evidence: per band a start and per-line slope (offset / $3720, $14A0, $1C20, $1950 << 4) through the unrolled line fill $B934; bands $58 - ViewY, $10, 8, $40, $30 + ViewY lines; only caller Stage1_UpdateStreetScroll
; ---------------------------------------------------------------------------
Stage1_ScrollFacadeLines:
	move.l	(g_Stage1SegOfs).w,d2
	move.l	d2,d1
	divs.w	#$3720,d1
	ext.l	d1
	asl.l	#$4,d1
	neg.l	d1
	move.l	d2,d0
	addi.l	#$FF508000,d0
	move.w	#$58,d7
	move.w	(g_ViewY).w,d6
	sub.w	d6,d7
	bra.s	loc_015DB6
loc_015DB4:
	add.l	d1,d0
loc_015DB6:
	dbra	d6,loc_015DB4
	lea.l	(g_HScrollLinesB).l,a0
	jsr	(Mem_FillRamp).l
	move.l	d0,d3
	move.l	d1,d4
	move.l	d2,d1
	divs.w	#$14A0,d1
	ext.l	d1
	asl.l	#$4,d1
	move.w	#$10,d7
	jsr	(Mem_FillRamp).l
	sub.l	d1,d0
	move.l	d2,d1
	divs.w	#$1C20,d1
	ext.l	d1
	asl.l	#$4,d1
	neg.l	d1
	move.w	#$8,d7
	jsr	(Mem_FillRamp).l
	move.l	d3,d0
	move.l	d4,d1
	muls.w	#$18,d4
	add.l	d4,d0
	move.w	#$40,d7
	jsr	(Mem_FillRamp).l
	move.l	d2,d1
	divs.w	#$1950,d1
	ext.l	d1
	asl.l	#$4,d1
	move.w	#$30,d7
	add.w	(g_ViewY).w,d7
	jmp	(Mem_FillRamp).l
; ---------------------------------------------------------------------------
; Stage1_StartFireAt - trigger callback: sets the stage 1 building fire at the upper-floor window nearest a world x and starts it if none burns
; In:  d4 = world x
; Out: $F512 = fire x in facade coordinates (x * 35/32 snapped to window $18/$50/$88/$D0/$108 of its $168 segment); $F518 = 2 when it was 0; d0 = 0 (trigger done)
; Evidence: callback longs in the street trigger table Tbl_015F5A ($16158-$16182) and $20D98 (camera x + $A0); the fire appears over the bank in the TAS from f2160 ($F518 0 -> 1)
; ---------------------------------------------------------------------------
Stage1_StartFireAt:
	moveq	#$0,d0
	move.w	d4,d0
	move.l	d0,d1
	lsr.l	#$3,d1
	add.l	d1,d0
	lsr.l	#$2,d1
	sub.l	d1,d0
	moveq	#$0,d1
	move.w	d0,d1
	divu.w	#$168,d1
	swap	d1
	sub.w	d1,d0
	move.w	#$18,d2
	cmpi.w	#$48,d1
	ble.s	loc_015E68
	move.w	#$50,d2
	cmpi.w	#$80,d1
	ble.s	loc_015E68
	move.w	#$88,d2
	cmpi.w	#$C0,d1
	ble.s	loc_015E68
	move.w	#$D0,d2
	cmpi.w	#$F8,d1
	ble.s	loc_015E68
	move.w	#$108,d2
loc_015E68:
	add.w	d2,d0
	move.w	d0,(g_Stage1FireX).w
	tst.b	(g_Stage1FireState).w
	bne.s	loc_015E7A
	move.b	#$2,(g_Stage1FireState).w
loc_015E7A:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Stage1_SwayFireLines - sways the stage 1 building fire: writes the plane A line scroll of the fire rows from the bottom up with a sine-driven lean, when the fire is on screen
; In:  $F512 fire x, $F506 facade x, $F4F8 frame x offset, g_ViewY
; Out: g_HScrollLinesA lines 0..$50-ViewY; $F510 sway phase += 2
; Evidence: d1 = sin($F510) - $2000 added per line, start offset from fire x - facade x (skipped when <= -$A0); branch from Stage1_UpdateStreetScroll when $F518 > 0 (TAS f2160-f4518)
; ---------------------------------------------------------------------------
Stage1_SwayFireLines:
	move.l	(g_Stage1FireX).w,d0
	sub.l	(g_SceneVars+$12).w,d0
	cmpi.l	#$FF600000,d0
	ble.s	loc_015EEC
	move.l	d0,d1
	divs.w	#$3720,d1
	ext.l	d1
	move.l	d1,d4
	muls.w	#$500,d4
	sub.l	d4,d0
	addq.w	#$2,(g_Stage1FirePhase).w
	move.w	(g_Stage1FirePhase).w,d1
	andi.w	#$3FE,d1
	lea.l	(Tbl_059B22).l,a0
	move.w	(a0,d1.w),d1
	ext.l	d1
	subi.l	#$2000,d1
	addi.l	#$8000,d0
	swap	d0
	add.w	(g_SceneVars+$4).w,d0
	swap	d0
	lea.l	(g_HScrollLinesA).l,a0
	move.w	#$50,d7
	sub.w	(g_ViewY).l,d7
	adda.w	d7,a0
	adda.w	d7,a0
	subq.w	#$1,d7
loc_015EE0:
	swap	d0
	move.w	d0,-(a0)
	swap	d0
	add.l	d1,d0
	dbra	d7,loc_015EE0
loc_015EEC:
	rts
; ---------------------------------------------------------------------------
; Stage1_WaveSkyLines - before any fire: ripples the top band of plane A (above the stage 1 street's building) with a sine wave that drifts each frame
; In:  $F506 facade x, g_ViewY, g_FrameParity
; Out: g_HScrollLinesA lines 0..$2F-ViewY = sin/4 - facade x (bit 0 set on odd frames); $F516 phase += 2
; Evidence: byte sine of Tbl_059B22 at phase + $18 per line, asr #2; branch from Stage1_UpdateStreetScroll when $F518 = 0 (TAS f532-f2160)
; ---------------------------------------------------------------------------
Stage1_WaveSkyLines:
	lea.l	(Tbl_059B22).l,a0
	lea.l	(g_HScrollLinesA).l,a1
	addq.w	#$2,(g_Stage1SkyPhase).w
	move.w	(g_Stage1SkyPhase).w,d1
	move.w	(g_SceneVars+$12).w,d2
	move.w	#$2F,d7
	move.w	(g_ViewY).l,d6
	sub.w	d6,d7
	mulu.w	#$18,d6
	add.w	d6,d1
	tst.b	(g_FrameParity).w
	bne.s	loc_015F3C
loc_015F1E:
	andi.w	#$3FE,d1
	move.b	(a0,d1.w),d0
	ext.w	d0
	asr.w	#$2,d0
	sub.w	d2,d0
	andi.w	#$FFFE,d0
	move.w	d0,(a1)+
	addi.w	#$18,d1
	dbra	d7,loc_015F1E
	rts
loc_015F3C:
	andi.w	#$3FE,d1
	move.b	(a0,d1.w),d0
	ext.w	d0
	asr.w	#$2,d0
	sub.w	d2,d0
	ori.w	#$1,d0
	move.w	d0,(a1)+
	addi.w	#$18,d1
	dbra	d7,loc_015F3C
	rts
