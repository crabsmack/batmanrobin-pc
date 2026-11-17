; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnSideToSide - spawns a tube-section jetpack trooper at a random speed that alternates between windows right ($50..$A0) and left (-$50..-$A0) of the targeted player
; In:  d4/d5 = x/y (wave entry, x $168 or -$28)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04557E: Op2_SetSpeedRand $33/$E6, coroutine 0 $455A6: Op2_GosubArgs ($50, -$50, $A0, $50) wait $960, ..., ($50, -$50, -$50, -$A0); wave entries $41CC4/$41CCC; RAM f122940/f123840 slot 0 $455BE; TAS 122888-127550 (11 spawns)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnSideToSide:
	lea.l	(Scr2_04557E).l,a2
	jmp	(Obj_SpawnEnemy).l
