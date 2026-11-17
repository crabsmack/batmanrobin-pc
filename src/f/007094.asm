; ---------------------------------------------------------------------------
; VM6_Start - starts an a6 loader script: stores its pc and sets the resume hook to VM6_Resume so Frame_End runs it from the next frame
; In:  a6 = script
; Out: g_A6ScriptPc = a6, g_A6ResumeHook = $70A8, d0 = 0
; Evidence: callers VM6_StartFromTrigger (trigger callback) and $33420 (a6 = Tbl_0330FC, stage 2 section setup); unlike Op2_StartA6Script it does not set g_A6ScriptBusy (VM6_Next does once it runs); 3 corpus runs
; ---------------------------------------------------------------------------
VM6_Start:
	move.l	a6,(g_A6ScriptPc).w
	move.l	#$70A8,(g_A6ResumeHook).w
	moveq	#$0,d0
	rts
