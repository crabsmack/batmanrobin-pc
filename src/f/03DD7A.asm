; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdSpawn5 - road schedule spawn kind 5: picks Boss_Hatter_SpawnRoadSplitRightSoon, then the spawn commands' common tail (lane, wait, spawn)
; In:  a0/d4/d1 as Boss_Hatter_RoadCmdTbl; a4 = the boss's VM block
; Out: a1 = the spawner; then Boss_Hatter_RoadCmdSpawnInLane's
; Evidence: lea Boss_Hatter_SpawnRoadSplitRightSoon,a1 / bra.s Boss_Hatter_RoadCmdSpawnInLane, Boss_Hatter_RoadCmdSpawn0's shape; the bra.s of Boss_Hatter_RoadCmdTbl entry 13; the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdSpawn5:
	lea.l	(Boss_Hatter_SpawnRoadSplitRightSoon).l,a1
	bra.s	Boss_Hatter_RoadCmdSpawnInLane
