; ---------------------------------------------------------------------------
; Flight_ScrollCloudBank - plane A cloud bank of the flying section: scrolls g_LayerA 1 px right per frame and to g_ViewY (Layer_ScrollStep), then sets plane A's V-scroll and a wobbling line H-scroll: each line = the wobble table (g_SceneWorkBuf+$400) at (V-scroll row*2 + phase g_SceneVars+$A0, +2 a frame) minus (layer x - $20)
; In:  -
; Out: g_VScrollA, g_HScrollLinesA (224 lines, unrolled at $334E2)
; Evidence: called by Flight_FrameCorridorToClouds/Flight_FrameClouds; TAS f46291-f49938 (3455 frames); png f047400: the big foreground clouds ripple sideways
; ---------------------------------------------------------------------------
Flight_ScrollCloudBank:
	lea.l	(g_LayerA).l,a6
	move.w	#$1,$4(a6)
	move.w	(g_ViewY).l,d0
	sub.w	$2(a6),d0
	move.w	d0,$6(a6)
	jsr	(Layer_ScrollStep).l
	bra.w	Flight_CloudBankLines
