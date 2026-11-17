; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadSplitRight - road schedule spawn kind 1: a road object (body $03DE16) that after $4B0 ticks bursts into three pieces flying right (cruise x $A00)
; In:  d4 = the lane's road x, d5 = y (replaced by the road distance); a4 = the boss's VM block
; Out: a6 = the object, d0 = 0 (the placement); a full pool quits the caller (Obj_SpawnEnemyOrQuit returns past this)
; Evidence: lea script / jsr Obj_SpawnEnemyOrQuit / jmp Stage3_RoadPlaceAtHorizon(pc); named by the lea at $03DD5A (Boss_Hatter_RoadCmdSpawn1, entry 9 of Boss_Hatter_RoadCmdTbl); the Hatter's road schedules (StartCoroutine2Rel targets $03F58C-$03FAB0 from the boss scripts $03E11A-$03F25A) use these kinds; the corpus ran kind 0 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadSplitRight:
	lea.l	(Scr2_03FB34).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage3_RoadPlaceAtHorizon(pc)
