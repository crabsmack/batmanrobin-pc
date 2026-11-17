; ---------------------------------------------------------------------------
; ScrollCmd_LayerA - parallax command 4: fills plane A's line H-scroll buffer in bands from a map layer's position and sets g_VScrollA to the layer's y
; In:  a5 -> long band table, long layer pointer
; Out: $FFE1D2 (224 line words) filled, g_VScrollA = layer y; continues with Scroll_NextCmd
; Evidence: a0 = $FFE1D2 (DMA'd to VRAM $BC00, plane A line scroll), Tbl_00970A passes table $971C and g_LayerA $FFED9C; TAS f6362-f101674
; ---------------------------------------------------------------------------
ScrollCmd_LayerA:
	lea.l	(g_HScrollLinesA).l,a0
	movea.l	(a5)+,a1
	movea.l	(a5)+,a6
	bsr.w	Scroll_BandsFromLayer
	move.w	d3,(g_VScrollA).w
	bra.s	Scroll_NextCmd
