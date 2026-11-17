; ---------------------------------------------------------------------------
; Continue_DrawCreditsText - per-frame text of the continue screen: runs the text map Tbl_049428 ('CREDITS n' with the g_Credits number, blinking PRESS START TO CONTINUE) and DMAs 3 rows of the image to plane B row 24
; In:  a2 script context (coroutine 1 of Scr2_0491A0, Op2_Native at $49210)
; Out: jsr $7FE.w (Op2_Return: the coroutine restarts here next frame)
; Evidence: TextMap_Run Tbl_049428 (text bytes 'CREDITS ', number op $A0 on $F64E, 'PRESS START TO CONTINUE'), TextMap_UploadRows d7 = 2, g_PlaneBVRAM + $C00; png scenario-game-over f42600 'CREDITS 6' below Mr. Freeze's head; f41999-f42598
; ---------------------------------------------------------------------------
Continue_DrawCreditsText:
	lea.l	(Tbl_049428).l,a0
	jsr	(TextMap_Run).l
	moveq	#$2,d7
	move.w	(g_PlaneBVRAM).w,d2
	addi.w	#$C00,d2
	jsr	(TextMap_UploadRows).l
	jsr	($7FE).w
