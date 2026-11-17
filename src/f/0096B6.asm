; ---------------------------------------------------------------------------
; Scroll_SetPlaneA - sets plane A's scroll words g_HScrollA / g_VScrollA from the map layer g_LayerA
; In:  none
; Out: g_HScrollA = d0, g_VScrollA = d1
; Evidence: a6 = g_LayerA, Scroll_LayerToScroll turns tile column/row $32/$34 and fine bits into pixels; g_HScrollA/g_VScrollA go to VRAM $BC00 / VSRAM 0 in $AF10; callers $1FECC, $48C36; TAS f391-f21099
; ---------------------------------------------------------------------------
Scroll_SetPlaneA:
	lea.l	(g_LayerA).l,a6
	bsr.w	Scroll_LayerToScroll
	move.w	d0,(g_HScrollA).w
	move.w	d1,(g_VScrollA).w
	rts
