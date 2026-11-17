; ---------------------------------------------------------------------------
; GameOver_ScrollCity - game-over per-frame native: moves the camera 2 px right and runs the parallax command list Tbl_049616, so the skyline scrolls by
; In:  -
; Out: g_CameraX += 2; Scroll_RunParallax (scroll mode $C)
; Evidence: Op2_CallNativeRel from the director Scr2_0494EA ($49504); scenario-game-over f42833-f43858 (816 frames)
; ---------------------------------------------------------------------------
GameOver_ScrollCity:
	addq.w	#$2,(g_CameraX).w
	lea.l	(Tbl_049616).l,a5
	jmp	(Scroll_RunParallax).l
