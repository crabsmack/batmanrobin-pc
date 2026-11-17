; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdSetLoop - command 5: sets the loop count (g_SceneVars+6, a byte) to byte 1 and reads the next command
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
; Out: loop = d1; then Boss_Hatter_RoadCmdNext
; Evidence: move.b d1,(g_SceneVars+$6).w / bra.s Boss_Hatter_RoadCmdNext; reached by Boss_Hatter_RoadCmdTbl entry 5 ($03DD3C); the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdSetLoop:
	move.b	d1,(g_SceneVars+$6).w
	bra.s	Boss_Hatter_RoadCmdNext
