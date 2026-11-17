; ---------------------------------------------------------------------------
; Enemy_JetTrooper_RideTubeScroll - native in the jetpack trooper's body loop: moves the trooper with the ring tunnel's scroll (16.16 x += g_SceneVars+4, the tunnel speed)
; In:  a6 = the trooper
; Out: d0 = the tunnel speed; the trooper's x; back to the script
; Evidence: move.l (g_SceneVars+$4).w,d0 / add.l d0,$3E(a6) / jsr $7FE.w; Op2_Native at $04241E in the body loop $0423CE; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_RideTubeScroll:
	move.l	(g_SceneVars+$4).w,d0
	add.l	d0,$3E(a6)
	jsr	($7FE).w
