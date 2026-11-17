; ---------------------------------------------------------------------------
; Boss_Claw_DrawPoints - draws d7+1 3D points of the claw as depth-scaled ball sprites: projects each (g_Rot3DOuter), adds the claw's x/y, frame = Tbl_01EBCE[size / depth], sorted by $2A(a6) - depth
; In:  a1 = point list, d7 = count - 1, a5 = sprite frame slots, a6 = the claw ($2A sort base, $2C attr bits)
; Out: a1/a5 advanced
; Evidence: Spr_ChangeFrame + Spr_AddSorted per point; called with the finger blocks (d7 = 2) by Boss_Claw_Draw; 2375 TAS frames
; ---------------------------------------------------------------------------
Boss_Claw_DrawPoints:
	move.w	d7,-(a7)
	lea.l	(g_Rot3DOuter).l,a0
	jsr	(Rot3D_ProjectPoint).l
	asr.w	#$1,d7
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
; ---------------------------------------------------------------------------
; Boss_Claw_DrawProjected - the Boss_Claw_DrawPoints loop after its projection: draws the projected point d4/d5/d7 as a depth-scaled ball, then pops the count and loops back to Boss_Claw_DrawPoints for the next
; In:  d4/d5 = projected x/y (world), d7 = depth / 2, a1 = the point's size word, a5 = sprite slots, a6 = the claw; the count on the stack
; Out: a1/a5 advanced; the pushed count popped into d7
; Evidence: bra target of Boss_Claw_DrawChain (which pushes the count and projects the chain head itself); moveq #0,d1 / move.w (a1)+,d1 / divu.w d7,d1 / Tbl_01EBCE frame / Spr_ChangeFrame / Spr_AddSorted / move.w (a7)+,d7 / dbra; 2349 TAS frames
; ---------------------------------------------------------------------------
Boss_Claw_DrawProjected:
	moveq	#$0,d1
	move.w	(a1)+,d1
	divu.w	d7,d1
	add.w	d1,d1
	move.w	Tbl_01EBCE(pc,d1.w),d1
	move.w	(a1)+,d6
	move.w	(a5),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a5)+
	neg.w	d7
	add.w	$2A(a6),d7
	or.w	$2C(a6),d6
	jsr	(Spr_AddSorted).l
	move.w	(a7)+,d7
	dbra	d7,Boss_Claw_DrawPoints
	rts
