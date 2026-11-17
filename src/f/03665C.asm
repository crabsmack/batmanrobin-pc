; ---------------------------------------------------------------------------
; Enemy_RabbitPot_SpawnA - spawner of the rabbit pot (Scr2_036668: energy $180, score $12C, body Scr4_03575E): it slides in from the right to its screen x (cruise $300) and puffs bursts of 20 shots (a4 $3596E from its mouth) forever
; In:  d4/d5 = position (y forced to screen $73)
; Out: Obj_SpawnEnemy's result
; Evidence: wave spawn entries $0347EC/$0347F4/$0348B6/$0348BE; TAS f111166-f112916; png f111600: the blue/copper striped pot (with a bomb rabbit climbing out)
; ---------------------------------------------------------------------------
Enemy_RabbitPot_SpawnA:
	lea.l	(Scr2_036668).l,a2
	jmp	(Obj_SpawnEnemy).l
