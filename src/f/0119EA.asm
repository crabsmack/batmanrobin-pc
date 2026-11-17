; ---------------------------------------------------------------------------
; HUD_StartGoSign - starts the GO! sign entering from the right (x $168, speed -4.0 px/frame) with the camera lock released on arrival
; In:  none
; Out: g_GoSignKeepLock = 0, g_GoSignX, g_GoSignSpeed, g_GoSignState = -1 (moving), g_GoSignCameraX = g_CameraX, d0 = 0
; Evidence: called by HUD_ShowGoSignCb; 20 times in the TAS (1388-120817), each followed by the GO! sprite (screenshot TAS f1460 shows GO!)
; ---------------------------------------------------------------------------
HUD_StartGoSign:
	sf.b	(g_GoSignKeepLock).w
; ---------------------------------------------------------------------------
; HUD_StartGoSign_set - shared tail of the two GO! sign starts: sets the start x, speed, moving state and the camera x to wait past
; In:  none (g_GoSignKeepLock already set or cleared)
; Out: as HUD_StartGoSign
; Evidence: fall-through of HUD_StartGoSign and the bra from HUD_StartGoSignKeepLock
; ---------------------------------------------------------------------------
HUD_StartGoSign_set:
	move.w	#$168,(g_GoSignX).w
	move.l	#$FFFC0000,(g_GoSignSpeed).w
	move.w	#$FFFF,(g_GoSignState).w
	move.w	(g_CameraX).w,(g_GoSignCameraX).w
	moveq	#$0,d0
	rts
