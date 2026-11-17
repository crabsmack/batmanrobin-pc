; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdSpawnInLane - common tail of the road schedule's spawn commands: stores the wait (byte 1 rows) and the resume pointer, picks the road x of lane n = high nibble (56n - 28; lane 0 = a random one of the 7) and calls the kind's spawner, then goes back to the wait countdown
; In:  a1 = spawner, a0 = past the command, d4 = byte 0, d1 = byte 1, a4 = the boss's a4 block
; Out: g_SceneVars+$10 = wait, a4 +$40 = a0; spawner called with d4 = lane x
; Evidence: Rand_Below(7) x 56 + $1C or n*64 - n*8 - $1C; movem a0/a4 around jsr (a1); TAS f114753-f114833
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdSpawnInLane:
	move.b	d1,(g_SceneVars+$10).w
	move.l	a0,$40(a4)
	andi.w	#$F0,d4
	bne.s	loc_03DDB4
	moveq	#$7,d0
	jsr	(Rand_Below).l
	asl.w	#$3,d0
	move.w	d0,d4
	asl.w	#$3,d4
	sub.w	d0,d4
	addi.w	#$1C,d4
	bra.s	loc_03DDC2
loc_03DDB4:
	move.w	d4,d0
	add.w	d4,d4
	add.w	d4,d4
	asr.w	#$1,d0
	sub.w	d0,d4
	subi.w	#$1C,d4
loc_03DDC2:
	movem.l	a0/a4,-(a7)
	jsr	(a1)
	movem.l	(a7)+,a0/a4
	bra.w	Boss_Hatter_RoadSchedCountWait
