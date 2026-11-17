; ---------------------------------------------------------------------------
; Enemy_RabbitPot_SpawnD - rabbit pot (Scr2_036714) already in place that only releases bomb rabbits: random sequences of three (hopping / far-hopping) $3C0 ticks apart, then a $B40 pause
; In:  d4/d5 = position (y forced to screen $73)
; Out: Obj_SpawnEnemy's result
; Evidence: runtime entry (ctarget); TAS f111764; png f111600 (a rabbit leaving the pot)
; ---------------------------------------------------------------------------
Enemy_RabbitPot_SpawnD:
	lea.l	(Scr2_036714).l,a2
	jmp	(Obj_SpawnEnemy).l
