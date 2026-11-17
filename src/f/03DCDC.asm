; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdWait - command 1: waits byte 1 checker rows (g_SceneVars+$10) and stores the resume pointer in the boss's slot 2
; In:  a0 = past the command, d1 = byte 1, a4 = the boss's VM block
; Out: wait = d1, slot 2 ($40(a4)) = a0; rts (the step ends)
; Evidence: move.b d1,(g_SceneVars+$10).w / move.l a0,$40(a4) / rts; reached by Boss_Hatter_RoadCmdTbl entry 1 ($03DD34); the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdWait:
	move.b	d1,(g_SceneVars+$10).w
	move.l	a0,$40(a4)
	rts
