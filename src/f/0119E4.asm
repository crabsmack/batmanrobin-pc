; ---------------------------------------------------------------------------
; HUD_StartGoSignKeepLock - native callback: starts the GO! sign like HUD_StartGoSign but sets g_GoSignKeepLock, so the arrival does not clear the camera lock
; In:  none
; Out: d0 = 0; GO! sign state as HUD_StartGoSign
; Evidence: only pointer: script Scr2_0204D4 (op $2CBC, first op of a spawned enemy script); ran once in the TAS (frame 9596) and in one fuzz run; HUD_UpdateGoSign tests g_GoSignKeepLock before clearing g_CameraLockX
; ---------------------------------------------------------------------------
HUD_StartGoSignKeepLock:
	st.b	(g_GoSignKeepLock).w
	bra.s	HUD_StartGoSign_set
