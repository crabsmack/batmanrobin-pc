; ---------------------------------------------------------------------------
; Enemy_BombRabbit_SpawnHopPop4 - Enemy_BombRabbit_SpawnHop with a population weight of 4 (Op2_AddPopulation 4 instead of +1), so waves hold back while it lives (script Scr2_035D38)
; In:  d4/d5 = spawn position
; Out: Obj_SpawnEnemy's result
; Evidence: dc.l at $03891C (another part's wave entry); not in the TAS; level-skip scenario f10651-f10831
; ---------------------------------------------------------------------------
Enemy_BombRabbit_SpawnHopPop4:
	lea.l	(Scr2_035D38).l,a2
	jmp	(Obj_SpawnEnemy).l
