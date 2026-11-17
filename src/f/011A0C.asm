; ---------------------------------------------------------------------------
; HUD_GoSignLeave - sends the stopped GO! sign off to the right: speed +$1A00 and a hold timer of $47 counted down by 2 before it moves
; In:  none
; Out: g_GoSignSpeed = $1A00, g_GoSignState = $47
; Evidence: only caller HUD_UpdateGoSign, once the camera has scrolled past g_GoSignCameraX; ran 20 times in the TAS (1431-120859), ~43 frames after each HUD_StartGoSign
; ---------------------------------------------------------------------------
HUD_GoSignLeave:
	move.l	#$1A00,(g_GoSignSpeed).w
	move.w	#$47,(g_GoSignState).w
	rts
