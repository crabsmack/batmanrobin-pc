; ---------------------------------------------------------------------------
; Boss_Claw_DrawChain - projects the chain head to set the claw's drawn position $4E/$50 and its hittable flag $2E (depth $700-$800), picks front (sort $4000, priority) or back (sort $1000) by depth <= $780, then draws d7+1 chain balls
; In:  a1 = point list (x, y, z, size, attr), d7 = balls - 1, a5 = sprite slots, a6 = the claw
; Out: as Boss_Claw_DrawPoints
; Evidence: Rot3D_ProjectPoint with g_Rot3DOuter; falls into the Boss_Claw_DrawPoints loop at $1EB9E; clears $F6D2 (no reader found); 2349 TAS frames
; ---------------------------------------------------------------------------
Boss_Claw_DrawChain:
	move.w	d7,-(a7)
	lea.l	(g_Rot3DOuter).l,a0
	jsr	(Rot3D_ProjectPoint).l
	asr.w	#$1,d7
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	move.w	d4,$4E(a6)
	move.w	d5,$50(a6)
	clr.w	$2E(a6)
	cmpi.w	#$700,d7
	blt.s	loc_01EB60
	cmpi.w	#$800,d7
	bgt.s	loc_01EB60
	addq.w	#$1,$2E(a6)
loc_01EB60:
	clr.w	(g_UnkClaw_F6D2).l
	cmpi.w	#$780,d7
	bgt.s	loc_01EB7A
	move.w	#$4000,$2A(a6)
	move.w	#$8000,$2C(a6)
	bra.s	Boss_Claw_DrawProjected
loc_01EB7A:
	move.w	#$1000,$2A(a6)
	clr.w	$2C(a6)
	bra.s	Boss_Claw_DrawProjected
