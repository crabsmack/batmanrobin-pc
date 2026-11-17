; ---------------------------------------------------------------------------
; ScrollCmd_CameraA - parallax command $C: fills plane A's line H-scroll buffer in bands from the view position g_ViewX/g_ViewY and sets g_VScrollA
; In:  a5 -> long band table
; Out: $FFE1D2 lines filled, g_VScrollA = g_ViewY; continues with Scroll_NextCmd
; Evidence: d2 = g_ViewX (16.16) >> 8, d3 = g_ViewY integer word, then Scroll_FillBands; TAS f108331-f113271
; ---------------------------------------------------------------------------
ScrollCmd_CameraA:
	lea.l	(g_HScrollLinesA).l,a0
	movea.l	(a5)+,a1
	move.l	(g_ViewX).w,d2
	asr.l	#$8,d2
	move.w	(g_ViewY).w,d3
	bsr.w	Scroll_FillBands
	move.w	d3,(g_VScrollA).w
	bra.s	Scroll_NextCmd
