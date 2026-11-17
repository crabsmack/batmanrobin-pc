; ---------------------------------------------------------------------------
; Continue_UseCredit - the Start case of the continue screen's Pad_CheckStart switch: takes one credit, then the script ends the scene with result 1 (continue) after 30 frames and keeps only the credits text running
; In:  a2 script context (case 1 of the op $2D40 switch at $491F0)
; Out: g_Credits - 1; jsr $7FE.w (Scr2_049204: Scene_EndAfterDelay result 1, $1E frames)
; Evidence: subq.w #1,g_Credits; ran only in scenario-continue (f42201), where the game restarts the section (Main_PlayLoop: result 1 -> Tbl_00A05C)
; ---------------------------------------------------------------------------
Continue_UseCredit:
	subq.w	#$1,(g_Credits).w
	jsr	($7FE).w
