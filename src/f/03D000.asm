; ---------------------------------------------------------------------------
; Stage3_Line3CycleStep - one step of the line-3 colour walker: blue d2 += d5 and bounces between 0 and $E00; only when it reaches an end does green d1 += d4 (bounces 0-$E0), and only at green's ends red d0 += d3 (0-$E)
; In:  d0/d1/d2 = red/green/blue components (CRAM bit positions), d3/d4/d5 = their steps
; Out: d0-d5 advanced (a step negated at its bound)
; Evidence: bsr from Stage3_StepLine3Cycle 16 times per frame (colours 1-15 of palette line 3); bounds $E/$E0/$E00 = Genesis colour components; TAS f97888-f101674 (1405 frames)
; ---------------------------------------------------------------------------
Stage3_Line3CycleStep:
	add.w	d5,d2
	beq.s	loc_03D00A
	cmpi.w	#$E00,d2
	bne.s	loc_03D024
loc_03D00A:
	neg.w	d5
	add.w	d4,d1
	beq.s	loc_03D016
	cmpi.w	#$E0,d1
	bne.s	loc_03D024
loc_03D016:
	neg.w	d4
	add.w	d3,d0
	beq.s	loc_03D022
	cmpi.w	#$E,d0
	bne.s	loc_03D024
loc_03D022:
	neg.w	d3
loc_03D024:
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_Spawn - spawns the Mad Hatter boss, Scr2_03D032 (boss class): the giant upside-down top hat with the Hatter on its brim; its script makes the road-steering child (field $12), sets score 5000 and energy $1D00/$2D00 (1P/2P), shows the boss meter, sets up two sprite blocks and four coroutines, steps the road schedule every frame (Boss_Hatter_StepRoadSchedule) and on defeat wipes the enemies, explodes and bounces
; In:  -
; Out: a6 = the boss (Obj_SpawnBoss)
; Evidence: jsr from Stage3_InitHatterRoad; Scr2_03D032: Op2_SpawnChildOrWait $12 / $03DA64, Op2_SetScoreValue $1388, Op2_SetEnergy1P2P, BossMeter_Show, Op2_CallNativeRelSaved $03DC94, BossMeter_Defeated, Hitbox_WipeEnemies; png f114600: the Hatter standing on the hat's brim
; ---------------------------------------------------------------------------
Boss_Hatter_Spawn:
	lea.l	(Scr2_03D032).l,a2
	jmp	(Obj_SpawnBoss).l
