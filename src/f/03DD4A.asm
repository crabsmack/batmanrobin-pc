; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdTbl_Spawn4 - entry 12 of Boss_Hatter_RoadCmdTbl: spawn kind 4 (Boss_Hatter_SpawnRoadDriftLeft), a bra.s to Boss_Hatter_RoadCmdSpawn4
; In:  as Boss_Hatter_RoadCmdSpawn4
; Out: as Boss_Hatter_RoadCmdSpawn4
; Evidence: bra.s at $03DD4A = $03DD32 + 12 x 2; Boss_Hatter_RoadCmdNext's `jmp Boss_Hatter_RoadCmdTbl(pc,d0.w)` with d0 = (byte 0 & $F) x 2, so all 16 entries are reachable; the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdTbl_Spawn4:
	bra.s	Boss_Hatter_RoadCmdSpawn4
