; ---------------------------------------------------------------------------
; ScrollCmd_CameraB - parallax command $14: fills plane B's line H-scroll buffer in bands from the view position and sets the plane B V-scroll word
; In:  a5 -> long band table
; Out: $FFE392 lines filled, $DFB8 = g_ViewY; continues with Scroll_NextCmd
; Evidence: a0 = $FFE392 (DMA'd to VRAM $BC02, plane B line scroll); Tbl_00970A's table $9720 = bands $60 lines at x/2.5, $20 at x/1.5, rest at x/0.75; TAS f6362-f119385
; ---------------------------------------------------------------------------
ScrollCmd_CameraB:
	lea.l	(g_HScrollLinesB).l,a0
	movea.l	(a5)+,a1
	move.l	(g_ViewX).w,d2
	asr.l	#$8,d2
	move.w	(g_ViewY).w,d3
	bsr.w	Scroll_FillBands
	move.w	d3,(g_VScrollB).w
	bra.w	Scroll_NextCmd
; ---------------------------------------------------------------------------
; ScrollCmd_CameraOfsB - parallax command $18: like ScrollCmd_CameraB but adds a 16.16 x offset from the command list to the view x first
; In:  a5 -> long band table, long x offset
; Out: $FFE392 lines filled, $DFB8 = g_ViewY; continues with Scroll_NextCmd
; Evidence: add.l (a5)+,d2 before the >> 8; the only difference from ScrollCmd_CameraB
; ---------------------------------------------------------------------------
ScrollCmd_CameraOfsB:
	lea.l	(g_HScrollLinesB).l,a0
	movea.l	(a5)+,a1
	move.l	(g_ViewX).w,d2
	add.l	(a5)+,d2
	asr.l	#$8,d2
	move.w	(g_ViewY).w,d3
	bsr.w	Scroll_FillBands
	move.w	d3,(g_VScrollB).w
	bra.w	Scroll_NextCmd
