; ---------------------------------------------------------------------------
; Enemy_RabbitPot_SpawnC - rabbit pot (Scr2_0366F0) already in place: lets out a hopping rabbit, waits $780, another, waits $3C0, puffs 30 shots, then picks at random between more rabbit pairs (hopping or far-hopping)
; In:  d4/d5 = position (y forced to screen $73)
; Out: Obj_SpawnEnemy's result
; Evidence: runtime entry (ctarget, no long pointer: a wave entry built elsewhere); TAS f111557-f111640
; ---------------------------------------------------------------------------
Enemy_RabbitPot_SpawnC:
	lea.l	(Scr2_0366F0).l,a2
	jmp	(Obj_SpawnEnemy).l
