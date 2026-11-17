; ---------------------------------------------------------------------------
; HudOp_SlideOut - panel script op $C: starts the panel sliding off its screen edge (calls $118E2) and keeps running
; In:  a6 = player record
; Out: +$12/+$16 slide speed, +$25 bit 6 set
; Evidence: entry $C of Tbl_011764; $118E2 aims at home x +$A -/+ $68 (side from +$25 bit 2) from the current x +$E; scripts use it before switching text/stats (Tbl_010F76, $10FB2) (direction inferred from the arithmetic)
; ---------------------------------------------------------------------------
HudOp_SlideOut:
	bsr.w	HUD_StartSlideOut
	bra.s	HUD_PanelNext
