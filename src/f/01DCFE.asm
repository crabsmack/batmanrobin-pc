; ---------------------------------------------------------------------------
; Boss_JokerBalloon_ThrowByArg - balloon boss native: takes entry g_VM2Arg0 of the 8-byte table $1DD48 (dx, dy, heading, kind), runs the puff a4 script $1DDA0 at the boss's x/y + (dx, dy) (Fx_RunScript) and spawns that kind's thrown object there (Boss_JokerBalloon_SpawnThrown)
; In:  a6 = the boss (+$3E/$42 x/y); g_VM2Arg0 high word = the entry
; Out: a1-a6 kept (movem); d2-d6 = the entry's values (d4/d5 the position as 16.16), d0 = Boss_JokerBalloon_SpawnThrown's result
; Evidence: Op2_CallNativeRel at $01D842 in the Joker balloon boss's script; movem.l a1-a6 / move.w g_VM2Arg0,d0 / lsl #3 / lea $1DD48(pc,d0) / Fx_RunScript $1DDA0 / bsr $01DE6A; raw until crane-crash (2026-10-30): no corpus run reaches it
; ---------------------------------------------------------------------------
Boss_JokerBalloon_ThrowByArg:
	movem.l	a1-a6,-(a7)
	move.w	(g_VM2Arg0).w,d0
	lsl.w	#$3,d0
	lea.l	Tbl_01DD48(pc,d0.w),a0
	moveq	#$0,d2
	moveq	#$0,d3
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	(a0)+,d4
	move.w	(a0)+,d5
	move.w	(a0)+,d3
	move.w	(a0)+,d6
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	swap	d3
	swap	d4
	swap	d5
	movem.l	d4-d6,-(a7)
	lea.l	(Scr4_01DDA0).l,a0
	jsr	(Fx_RunScript).l
	movem.l	(a7)+,d4-d6
	bsr.w	Boss_JokerBalloon_SpawnThrown
	movem.l	(a7)+,a1-a6
	rts
