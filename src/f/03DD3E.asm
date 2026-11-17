; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdTbl_Loop - entry 6 of Boss_Hatter_RoadCmdTbl: command 6 (count the loop down, the goto while it stays above 0), a bra.s to Boss_Hatter_RoadCmdLoop
; In:  as Boss_Hatter_RoadCmdLoop
; Out: as Boss_Hatter_RoadCmdLoop
; Evidence: bra.s at $03DD3E = $03DD32 + 6 x 2; Boss_Hatter_RoadCmdNext's `jmp Boss_Hatter_RoadCmdTbl(pc,d0.w)` with d0 = (byte 0 & $F) x 2, so all 16 entries are reachable; the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdTbl_Loop:
	bra.s	Boss_Hatter_RoadCmdLoop
