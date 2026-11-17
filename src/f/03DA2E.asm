; ---------------------------------------------------------------------------
; Boss_Hatter_ThrownToyFrame - a4 native in the toy figures the Hatter throws: deletes the toy below y $F0 (pops the script return, Op4_Kill), else sets its frame and attributes from the rotation table at the road's spin index g_SceneVars2+$3E: Tbl_036DA8 (plain) or the outlined table at Tbl_ToyFigureChargedFrames on alternate 2-frame phases (g_FrameCounter bit 1)
; In:  a6 = the toy (a4 object), d5 = its drawn y
; Out: $1E (frame), $22 (attr = byte AND $E0 XOR flip byte)
; Evidence: Op4_CallNative dc.l $0003DA2E at $03DA1C in Scr4_03D9BC (the child of the throw scripts $03F494/$03F548, Op4_HitPlayers EnemyAtk_HitTouch), then Op4_DrawFrame; TAS f113792-f114052 (261 frames)
; ---------------------------------------------------------------------------
Boss_Hatter_ThrownToyFrame:
	cmpi.w	#$F0,d5
	blt.s	loc_03DA3C
	addq.l	#$4,a7
	jmp	(Op4_Kill).l
loc_03DA3C:
	lea.l	Tbl_036DA8(pc),a0
	btst	#$1,(g_FrameCounter+3).w
	beq.s	loc_03DA4C
	lea.l	Tbl_ToyFigureChargedFrames(pc),a0
loc_03DA4C:
	adda.w	(g_SceneVars2+$3E).w,a0
	move.w	(a0)+,$1E(a6)
	move.b	#$E0,d0
	and.b	(a0)+,d0
	move.b	(a0)+,d1
	eor.b	d1,d0
	move.b	d0,$22(a6)
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadSteer - spawns the Hatter's road-steering child Scr2_03DA8C (boss class; the boss keeps it in field $12): an invisible mover whose world x/y become the road's bend and pitch and whose script coroutines (started by the boss) set the road speed; stops its coroutine slots 0/1 and clears its script variables 0-2
; In:  -
; Out: a6 = the child (quits the caller when the pool is full)
; Evidence: Op2_SpawnChildOrWait $12 / dc.l $03DA64 at $03D032; Obj_SpawnBossOrQuit; slots $38/$3C = $2D28; Scr2_03DA8C: Op2_SetPos 0,0, Op2_AllocOsc, every frame Op2_StepVelocity/Op2_AddOsc/Op2_ApplyMove then Boss_Hatter_SteerRoad; TAS f113311
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadSteer:
	lea.l	(Scr2_03DA8C).l,a2
	jsr	(Obj_SpawnBossOrQuit).l
	move.l	#$2D28,$38(a4)
	move.l	#$2D28,$3C(a4)
	moveq	#$0,d0
	move.l	d0,$22(a4)
	move.l	d0,$24(a4)
	rts
