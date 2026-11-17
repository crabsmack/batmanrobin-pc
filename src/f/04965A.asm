; ---------------------------------------------------------------------------
; GameOver_TextKeepPace - native step of the GAME OVER text: adds 2 px to the frame's x move so the text keeps pace with the camera that GameOver_ScrollCity moves 2 px a frame
; In:  d1 = x move (16.16) from Op2_StepVelocity
; Out: d1 += $20000; jsr $7FE.w (Op2_ApplyMove, Text_DrawAtObject, yield)
; Evidence: addi.l #$20000,d1 between Op2_StepVelocity and Op2_ApplyMove in Scr2_04963C; GameOver_ScrollCity addq.w #2,g_CameraX; scenario-game-over f42833-f43858 (1025 frames)
; ---------------------------------------------------------------------------
GameOver_TextKeepPace:
	addi.l	#$20000,d1
	jsr	($7FE).w
