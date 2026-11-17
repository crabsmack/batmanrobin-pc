; ---------------------------------------------------------------------------
; Enemy_BombBot_FrameByCameraX - returns bit 1 of g_CameraX: picks one of the bomb bot's two rolling frames ($4A1A / $4A24) from the scroll position
; In:  none
; Out: d0 = 0 or 1
; Evidence: only reference: Op2_CallNativeRelSwitch at $4337E in the bomb bot animation Scr4_043378 (an a2 animation script, set by Op2_SetAnim at $432EC in the body $432E2); TAS 116192-122471 (stage 4 hall and lab)
; ---------------------------------------------------------------------------
Enemy_BombBot_FrameByCameraX:
	move.w	(g_CameraX).w,d0
	lsr.w	#$1,d0
	andi.w	#$1,d0
	rts
