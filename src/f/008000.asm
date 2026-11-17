; ---------------------------------------------------------------------------
; Spr_ReleaseFrame - drops one reference to the art of frame d2; at the last one its blocks count as free and its descriptor goes on the release ring for later eviction
; In:  d2 = frame (>= 0 ROM table Tbl_1DDB60, < 0 RAM table below $A764)
; Out: a0 clobbered; nothing when the frame's handle holds no art
; Evidence: same release code as the tail of Spr_SwapFrameArt; callers clear the frame word after it (e.g. $4508 `move.w (a1),d2 / bsr / clr.w (a1)+` when an object drops sprites), also $53D4, $9C4, $109A0
; ---------------------------------------------------------------------------
Spr_ReleaseFrame:
	lea.l	(Tbl_1DDB60).l,a0
	tst.w	d2
	bpl.s	loc_00800E
	lea.l	(g_ArtRelRingWr).w,a0
loc_00800E:
	movea.w	(a0,d2.w),a0
	move.w	(a0),d0
	beq.s	loc_008038
	movea.w	d0,a0
	addq.b	#$1,(a0)+
	bne.s	loc_00803A
	move.b	(a0)+,d2
	add.b	d2,(g_ArtBlocksFree).w
	addq.b	#$1,(g_ArtDescsFree).w
	tst.w	(a0)
	bne.s	loc_008038
	move.w	(g_ArtRelRingWr).w,d2
	addq.b	#$2,(g_ArtRelRingWrLo).w
	move.w	d2,(a0)
	movea.w	d2,a0
	move.w	d0,(a0)
loc_008038:
	rts
loc_00803A:
	bmi.s	loc_008038
	clr.b	-$1(a0)
	rts
