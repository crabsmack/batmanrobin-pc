; ---------------------------------------------------------------------------
; Fx_RideTubeScroll - a4 native of a ring-tunnel effect script: moves the effect with the tunnel's scroll (x += the speed's integer word)
; In:  a6 = the effect
; Out: d0.w = the speed's integer word; $2E(a6); back to the a4 script
; Evidence: move.w (g_SceneVars+$4).w,d0 / add.w d0,$2E(a6) / jsr $53C6.w; Op4_Native at $042918 in the a4 script $042900; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Fx_RideTubeScroll:
	move.w	(g_SceneVars+$4).w,d0
	add.w	d0,$2E(a6)
	jsr	($53C6).w
