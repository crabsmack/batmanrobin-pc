; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdTbl_Random - entry 7 of Boss_Hatter_RoadCmdTbl: command 7 (jump through a random entry of the word-offset table after it), a bra.s to Boss_Hatter_RoadCmdRandom
; In:  as Boss_Hatter_RoadCmdRandom
; Out: as Boss_Hatter_RoadCmdRandom
; Evidence: bra.s at $03DD40 = $03DD32 + 7 x 2; Boss_Hatter_RoadCmdNext's `jmp Boss_Hatter_RoadCmdTbl(pc,d0.w)` with d0 = (byte 0 & $F) x 2, so all 16 entries are reachable; the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdTbl_Random:
	bra.s	Boss_Hatter_RoadCmdRandom
