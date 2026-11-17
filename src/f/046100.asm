; ---------------------------------------------------------------------------
; Enemy_BombBot_Spawn - spawns a lab bomb bot (a silver capsule robot on a wheel, with an antenna) at floor height $A6: it rolls after the targeted player (cruise $155) and self-destructs when its random fuse (timer 0) runs out
; In:  d4/d5 = x/y (wave entry: x $154 or -$14)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04610C: population +2, score $C8, energy $100, Op2_SetTimer0Rand $12C0/$780, body $432E2: coroutine 2 Op2_CruiseToPlayerX + Op2_FacePlayerTurnBranch, coroutine 0 $43342 (Op2_LoopTimer0, stop, warning anim $433B4 with sound $44, $2D0 ticks, Op2_FxSpawnHere blast $433DA, Op2_Kill without score); 10 wave entries in Scr2_0417CC (half with spawn action $29); RAM f122100 objects of this body at x 4920/5118 (camera 4802), a silver robot and a smoking wreck in shot w29_c122100; TAS 119818-122363 (9 spawns)
; ---------------------------------------------------------------------------
Enemy_BombBot_Spawn:
	lea.l	(Scr2_04610C).l,a2
	jmp	(Obj_SpawnEnemy).l
