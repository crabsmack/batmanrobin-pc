; ---------------------------------------------------------------------------
; BossMeter_Spawn - spawns the boss energy meter object (BossMeter_SpawnObject) and keeps its handle in g_BossMeterObj
; In:  none
; Out: g_BossMeterObj = the new object and d0 = 0; on a failed spawn the nonzero result is returned without storing (inferred: flags from Obj_SpawnEnemy)
; Evidence: reached from BossMeter_Show when no meter exists; ran 13 times in the TAS (4540-128791), once per boss fight
; ---------------------------------------------------------------------------
BossMeter_Spawn:
	movem.l	a2-a6,-(a7)
	bsr.w	BossMeter_SpawnObject
	movea.l	a6,a0
	movem.l	(a7)+,a2-a6
	bne.s	loc_011B04
	move.w	a0,(g_BossMeterObj).w
	moveq	#$0,d0
loc_011B04:
	rts
