; ---------------------------------------------------------------------------
; Enemy_Gunman_SpawnWalkIn - spawns the trench-coat gunman in palette line 2 (energy $40, score $FA) that walks in and keeps $50-$78 px from its target player
; In:  d4/d5 = x/y (Tbl_0166EA +$430..+$56A)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01BF6A: ModSprAttr $98/$40, SetEnergy $40, SetRandJitter $20, coroutine 0 = $1C0A0 (BalancePlayerTarget, anim $10A, Op2_HoldRangeOfPlayerX $50,$78); TAS f1528-f9385
; ---------------------------------------------------------------------------
Enemy_Gunman_SpawnWalkIn:
	lea.l	(Scr2_01BF6A).l,a2
	jmp	(Obj_SpawnEnemy).l
