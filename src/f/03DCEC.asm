; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdLoop - command 6: counts the loop count down; while it stays above 0 the command is a goto (12-bit offset), else the next command
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
; Out: loop - 1; Boss_Hatter_RoadCmdGoto while above 0, else Boss_Hatter_RoadCmdNext
; Evidence: subq.b #1,(g_SceneVars+$6).w / bgt.s Boss_Hatter_RoadCmdGoto / bra.s Boss_Hatter_RoadCmdNext; reached by Boss_Hatter_RoadCmdTbl entry 6 ($03DD3E); the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdLoop:
	subq.b	#$1,(g_SceneVars+$6).w
	bgt.s	Boss_Hatter_RoadCmdGoto
	bra.s	Boss_Hatter_RoadCmdNext
