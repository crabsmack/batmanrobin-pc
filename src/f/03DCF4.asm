; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdGosub - command 3: pushes the pointer past the command on the schedule's gosub stack (g_SceneVars+8, depth +4) and falls into the goto
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
; Out: stack[depth] = a0, depth + 4, a1 = the slot; then Boss_Hatter_RoadCmdGoto
; Evidence: lea (g_SceneVars+$8).w,a1 / adda.w (g_SceneVars+$4).w,a1 / addq.w #4,(g_SceneVars+$4).w / move.l a0,(a1) / falls into Boss_Hatter_RoadCmdGoto; reached by Boss_Hatter_RoadCmdTbl entry 3 ($03DD38); the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdGosub:
	lea.l	(g_SceneVars+$8).w,a1
	adda.w	(g_SceneVars+$4).w,a1
	addq.w	#$4,(g_SceneVars+$4).w
	move.l	a0,(a1)
