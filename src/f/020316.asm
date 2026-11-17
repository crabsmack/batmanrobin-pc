; ---------------------------------------------------------------------------
; Stage1_LaunchBalloonC - launcher native: the balloon-bomb spawn of Stage1_LaunchBalloonA, its height cursor wrapping to the third table $020388
; In:  a6 = launcher; $1E(a6) = cursor into (word height1, word height2) pairs ended by a negative word
; Out: d0 = 0 and the balloon's $2A/$2C = the two heights, cursor advanced; d0 != 0 when the pool is full
; Evidence: Op2_CallNativeRel at $02024A, $020254, $02025E (a launcher script's three timed launches); the code of Stage1_LaunchBalloonA/B with lea $020388,a0; raw until crane-crash (2026-10-30): no corpus run reaches it
; ---------------------------------------------------------------------------
Stage1_LaunchBalloonC:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	movem.l	a2-a6,-(a7)
	lea.l	(Scr2_0203E0).l,a2
	jsr	(Obj_SpawnEnemy).l
	movea.l	a6,a1
	movem.l	(a7)+,a2-a6
	bne.s	loc_020352
	movea.l	$1E(a6),a0
	move.w	(a0)+,$2A(a1)
	move.w	(a0)+,$2C(a1)
	tst.w	(a0)
	bpl.s	loc_02034C
	lea.l	(Tbl_020388).l,a0
loc_02034C:
	move.l	a0,$1E(a6)
	moveq	#$0,d0
loc_020352:
	rts
