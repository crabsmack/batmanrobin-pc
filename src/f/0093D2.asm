; ---------------------------------------------------------------------------
; HUD_ShowGoSignCb - native callback: starts the GO! sign (HUD_StartGoSign) and returns d0 = 0
; In:  none
; Out: d0 = 0
; Evidence: HUD_StartGoSign sets the sign's x $168, speed -4.0 and state $ED7E = -1; $11A1C slides and animates it, and clears g_CameraLockX once the camera passes $ED80; screenshot TAS f1460 shows GO!; callers a5 op $6E9A and script $1EFAC
; ---------------------------------------------------------------------------
HUD_ShowGoSignCb:
	jsr	(HUD_StartGoSign).l
	moveq	#$0,d0
	rts
