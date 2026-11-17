; ---------------------------------------------------------------------------
; Enemy_RabbitPot_SpawnB - rabbit pot (Scr2_03669A) that slides in, puffs 30 shots, then lets out two hopping bomb rabbits ($780 ticks apart), and repeats
; In:  d4/d5 = position (y forced to screen $73)
; Out: Obj_SpawnEnemy's result
; Evidence: wave entries $0348C6/$0348CE; TAS f112995; the raw spawner $0366C0 (Scr2_0366CC, no slide-in, shots only) follows it
; ---------------------------------------------------------------------------
Enemy_RabbitPot_SpawnB:
	lea.l	(Scr2_03669A).l,a2
	jmp	(Obj_SpawnEnemy).l
