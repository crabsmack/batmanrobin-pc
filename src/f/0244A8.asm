; ---------------------------------------------------------------------------
; Stage2_LiftVIntScroll - V-int object of the lift section: when a frame is ready, builds the 20-column V-scroll of the shaft (plane A from g_SceneVars+$A, plane B from layer A's row) and the shake split line, then writes the H-scroll, DMAs the columns and arms the H-int stub Stage2_LiftHIntFloor at line $AF - shake
; In:  a0/a1 = VDP control/data (V-int object list)
; Out: g_SceneWorkBuf, $F5C4-$F5C8 set; g_HIntCode = jmp Stage2_LiftHIntFloor; VDP reg $8A = the split line
; Evidence: installed by Stage2_LiftInstallVInt; move.w #$4EF9,g_HIntCode / move.l #$24526 (NOTES 'Variant B' jmp stubs); TAS f21158-f32065 every frame
; ---------------------------------------------------------------------------
Stage2_LiftVIntScroll:
	tst.b	(g_FrameBusy).w
	bne.s	loc_0244F4
	move.w	(g_ShakeY).w,d0
	move.w	d0,(g_SceneVars2+$28).w
	move.w	d0,(g_SceneVars2+$2A).w
	neg.w	d0
	addi.w	#$AF,d0
	move.w	d0,(g_SceneVars2+$26).w
	lea.l	(g_LayerA).l,a5
	move.w	$34(a5),d0
	addq.w	#$2,d0
	lsl.w	#$3,d0
	move.w	$2(a5),d1
	andi.w	#$F,d1
	or.b	d1,d0
	lea.l	(g_SceneVars+$A).l,a3
	lea.l	(g_SceneWorkBuf).l,a4
	move.w	#$13,d7
loc_0244EC:
	move.w	(a3)+,(a4)+
	move.w	d0,(a4)+
	dbra	d7,loc_0244EC
loc_0244F4:
	move.l	#$7C000002,(a0)
	move.l	#$FFF80000,(a1)
	jsr	(Scroll_DMAColumns).l
	move.w	#$4EF9,(g_HIntCode).l
	move.l	#$24526,(g_HIntOperand).l
	move.l	#$80148A00,d0
	move.b	(g_SceneVars2+$27).w,d0
	move.l	d0,(a0)
	rts
; ---------------------------------------------------------------------------
; Stage2_LiftHIntFloor - H-int at the lift floor's top line: stops further H-ints, sets plane A's H-scroll to -$158 and all 20 column V-scrolls to the shake value, so the lift platform below the line stays put while the shaft scrolls
; In:  -
; Out: VDP registers/VRAM $BC00/VSRAM written; rte
; Evidence: jmp stub written to g_HIntCode by Stage2_LiftVIntScroll ($024510); $8AFF, $7C000002 + $FEA8, $40000010 + 10 longs of $F5C6; png f028800 shows the riveted floor fixed at the bottom
; ---------------------------------------------------------------------------
Stage2_LiftHIntFloor:
	move.l	a0,-(a7)
	move.l	d0,-(a7)
	lea.l	($C00000).l,a0
	move.l	#$8AFF8F04,$4(a0)
	move.l	#$7C000002,$4(a0)
	move.w	#$FEA8,(a0)
	move.l	#$40000010,$4(a0)
	move.l	(g_SceneVars2+$28).w,d0
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	(a7)+,d0
	movea.l	(a7)+,a0
	rte
; ---------------------------------------------------------------------------
; Stage2_ArenaVIntScroll - V-int object of the stage 2 boss arena (installed by Stage2_LiftArriveTop): when a frame is ready copies the split line $F5BC to $F5BE, then continues into Stage2_ArenaVIntUpload
; In:  a0 = VDP control port
; Out: via Stage2_ArenaVIntUpload
; Evidence: move.l #$2456A to the $F5C2 object in Stage2_LiftArriveTop; skips the copy while g_FrameBusy (bne to $0245B0)
; ---------------------------------------------------------------------------
Stage2_ArenaVIntScroll:
	tst.b	(g_FrameBusy).w
	bne.s	$245B0
	move.w	(g_SceneVars2+$1E).w,(g_SceneVars2+$20).w
