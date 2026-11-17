; ---------------------------------------------------------------------------
; Continue_DrawNoCreditsText - per-frame text of the continue screen with no credits left: runs the text map Tbl_049480 (NO CREDITS REMAINING) and DMAs 3 rows to plane B row 24
; In:  a2 script context (coroutine 1 of Scr2_0491A0, Op2_Native at $049234)
; Out: jsr $7FE.w (Op2_Return)
; Evidence: Continue_DrawCreditsText's code with Tbl_049480 ('NO CREDITS REMAINING'); StartCoroutine1Rel at $0491CA; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Continue_DrawNoCreditsText:
	lea.l	(Tbl_049480).l,a0
	jsr	(TextMap_Run).l
	moveq	#$2,d7
	move.w	(g_PlaneBVRAM).w,d2
	addi.w	#$C00,d2
	jsr	(TextMap_UploadRows).l
	jsr	($7FE).w
