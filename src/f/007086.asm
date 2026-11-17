; ---------------------------------------------------------------------------
; VM6_RunHook - runs the a6 loader script's per-frame resume hook: jumps to g_A6ResumeHook when it is set, otherwise returns
; In:  g_A6ResumeHook = code to resume (VM6_Resume or an op's own wait routine) or 0
; Out: whatever the hook does (the a6 VM continues until an op parks or the script ends)
; Evidence: only caller Frame_End ($9BF0, before releasing the frame to the V-int); 169 corpus runs, every TAS frame from 18; the hooks installed are $70A8, $70D2, $713E, $72D2, $72F4, $743A, $756E, $7590, $76AE
; ---------------------------------------------------------------------------
VM6_RunHook:
	move.l	(g_A6ResumeHook).w,d0
	bne.s	loc_00708E
	rts
loc_00708E:
	movea.l	d0,a0
	jmp	(a0)
