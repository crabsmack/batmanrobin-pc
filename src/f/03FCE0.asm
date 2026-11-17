; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadPieceRightSlow3 - one of the three pieces a Boss_Hatter_SpawnRoadSplitRightSoon road object bursts into: flies right (x accel $4AA, cruise $500) on the piece body $03DE04, linked right after its parent (Obj_SpawnEnemyAfter)
; In:  d4/d5 = the parent's position (Spawn_AtOwnPos, Op2_CallNative $008D66 in the parent's coroutine 0)
; Out: jumps to Obj_SpawnEnemyAfter with a2 = its script
; Evidence: lea script / jmp Obj_SpawnEnemyAfter; script: SetSpeed $100, SetAccelXForDist $4AA $30/$60, SetCruiseX $500, Goto $03DE04; dc.l of Op2_CallNative Spawn_AtOwnPos at $03FC9C in Boss_Hatter_SpawnRoadSplitRightSoon's coroutine 0; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadPieceRightSlow3:
	lea.l	(Scr2_03FCEC).l,a2
	jmp	(Obj_SpawnEnemyAfter).l
