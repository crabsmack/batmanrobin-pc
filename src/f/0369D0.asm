; ---------------------------------------------------------------------------
; Enemy_DropCarrier_SpawnFigures - spawner (Obj_SpawnEnemy) of a drop carrier that crosses at screen height $84 and drops toy figures (ToyFigure_Spawn) in bursts of four (Scr2_0369DC: energy $100, cruise x $2AA, population weight $A, shared carrier body $035AA6)
; In:  d4/d5 = position (spawn entry), a1 = entry
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0369DC
; Evidence: coroutine 0 $036AC4: Op2_WaitPopBelow $12, then 4 x Op2_CallNative Spawn_AtOwnOfs dc.l ToyFigure_Spawn (0, -5) $F0 ticks apart; Tbl_034142 (+$5C0, +$656); TAS f110781 (1 frame)
; ---------------------------------------------------------------------------
Enemy_DropCarrier_SpawnFigures:
	lea.l	(Scr2_0369DC).l,a2
	jmp	(Obj_SpawnEnemy).l
