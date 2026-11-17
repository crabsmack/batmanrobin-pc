; ---------------------------------------------------------------------------
; VInt_ScrollLinesARaster - scroll mode $28: DMAs plane A's line H-scroll, then swaps and re-arms the H-int raster (per-line V-scroll)
; In:  a0 = VDP control port, a2 = DMA command scratch
; Out: H-int armed on the newly filled table; reg 1 = $64
; Evidence: bsr Scroll_DMALinesA, bra HInt_SwapRearm; entry $28 ($AEA4); 6 corpus runs, TAS frames 113312-116145
; ---------------------------------------------------------------------------
VInt_ScrollLinesARaster:
	bsr.w	Scroll_DMALinesA
	bra.w	HInt_SwapRearm
; ---------------------------------------------------------------------------
; VInt_ScrollLinesColumns - scroll mode $2C: line H-scroll of both planes plus 2-cell column V-scroll of both planes (Scroll_DMAColumnsAB)
; In:  a0 = VDP control port, a2 = DMA command scratch
; Out: reg 1 = $64
; Evidence: bsr Scroll_DMALines, bsr Scroll_DMAColumnsAB; entry $2C; $3BB88 sets mode $2C and also jmps here directly; 14 corpus runs, TAS frames 87704-108293 for $B0A8
; ---------------------------------------------------------------------------
VInt_ScrollLinesColumns:
	bsr.w	Scroll_DMALines
	bsr.w	Scroll_DMAColumnsAB
	move.w	#$8164,(a0)
	rts
; ---------------------------------------------------------------------------
; VInt_ScrollLinesA - scroll mode $30: DMAs plane A's line H-scroll only
; In:  a0 = VDP control port, a2 = DMA command scratch
; Out: reg 1 = $64
; Evidence: bsr Scroll_DMALinesA; entry $30 ($AEAC); 5 corpus runs (scenario-options, level-skip, robin)
; ---------------------------------------------------------------------------
VInt_ScrollLinesA:
	bsr.w	Scroll_DMALinesA
	move.w	#$8164,(a0)
	rts
; ---------------------------------------------------------------------------
; VInt_ScrollLinesBRaster - scroll mode $34: DMAs plane B's line H-scroll, then swaps and re-arms the H-int raster
; In:  a0 = VDP control port, a2 = DMA command scratch
; Out: H-int armed; reg 1 = $64
; Evidence: bsr Scroll_DMALinesB, bra HInt_SwapRearm; entry $34; 16 corpus runs, TAS frames 35433-46289 (stage 2, before the flying section)
; ---------------------------------------------------------------------------
VInt_ScrollLinesBRaster:
	bsr.w	Scroll_DMALinesB
	bra.w	HInt_SwapRearm
; ---------------------------------------------------------------------------
; VInt_ScrollLinesRaster - scroll mode $38: VSRAM long, line H-scroll of both planes, then swaps and re-arms the H-int raster
; In:  a0/a1 = VDP ports, a2 = DMA command scratch
; Out: H-int armed; reg 1 = $64
; Evidence: mode $38 is set with HInt_InstallVScrollA by $334D2 and $471C0 (the beam/disc span effects drawn into g_HScrollLinesA + the H-int table); 15 corpus runs
; ---------------------------------------------------------------------------
VInt_ScrollLinesRaster:
	move.l	#$40000010,(a0)
	move.l	(g_VScrollA).w,(a1)
	bsr.w	Scroll_DMALines
	bra.w	HInt_SwapRearm
; ---------------------------------------------------------------------------
; VInt_ScrollPlanesRaster - scroll mode $3C: whole-plane V and H scroll longs, then swaps and re-arms the H-int raster
; In:  a0/a1 = VDP ports
; Out: H-int armed; reg 1 = $64
; Evidence: commands $40000010 / $7C000002 as in VInt_ScrollPlanes, bra HInt_SwapRearm; $3BB88 sets mode $3C and jmps here directly after HInt_InstallVScrollB/A; 4 corpus runs
; ---------------------------------------------------------------------------
VInt_ScrollPlanesRaster:
	move.l	#$40000010,(a0)
	move.l	(g_VScrollA).w,(a1)
	move.l	#$7C000002,(a0)
	move.l	(g_HScrollA).w,(a1)
	bra.w	HInt_SwapRearm
