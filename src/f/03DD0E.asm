; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdReturn - command 4: pops the schedule's gosub stack (depth - 4) into the schedule pointer and reads the next command
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
; Out: depth - 4, a0 = the popped pointer; then Boss_Hatter_RoadCmdNext
; Evidence: subq.w #4,(g_SceneVars+$4).w / lea (g_SceneVars+$8).w,a0 / adda.w (g_SceneVars+$4).w,a0 / movea.l (a0),a0 / bra.s Boss_Hatter_RoadCmdNext; reached by Boss_Hatter_RoadCmdTbl entry 4 ($03DD3A); the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdReturn:
	subq.w	#$4,(g_SceneVars+$4).w
	lea.l	(g_SceneVars+$8).w,a0
	adda.w	(g_SceneVars+$4).w,a0
	movea.l	(a0),a0
	bra.s	Boss_Hatter_RoadCmdNext
