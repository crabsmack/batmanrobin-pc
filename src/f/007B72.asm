; ---------------------------------------------------------------------------
; Spr_BuildSAT - walks the front list then the sorted display list and writes each frame's pieces into the sprite attribute buffer, clipped to the screen, at most 80 sprites
; In:  the list built by Spr_Add*; g_SprOffsetX/Y ($9DD8/$9DDA) added to every x/y; g_SATPtr, g_SprCount
; Out: SAT entries (y, size|link, tile, x) appended; g_SprCountLo ($9DD7) and g_SATPtr updated
; Evidence: called once per frame from $9BF0 before Spr_QueueSATDMA. Per entry: (a0) word = art handle (0 = art not loaded, skipped), tile = 6(descriptor, piece index) + piece attr; H/V flip (attr bits 11/12) select the swapped offset words; pieces with y >= $100 or x >= $160 after the offset are dropped, others get +$60
; ---------------------------------------------------------------------------
Spr_BuildSAT:
	movea.w	(g_SATPtr).w,a1
	move.w	(g_SprCount).w,d2
	cmpi.b	#$50,d2
	bge.s	loc_007BAC
	movea.w	(g_SprOffsetX).w,a4
	movea.w	(g_SprOffsetY).w,a5
	moveq	#$0,d3
	movea.w	(g_SprSortFirst).w,a6
	movea.w	(g_SprFrontEnd).w,a0
	cmpa.w	#$9E16,a0
	beq.s	loc_007BA0
	move.w	a6,-$1E(a0)
	lea.l	($FFFF9E04).w,a6
loc_007BA0:
	addq.w	#$6,a6
loc_007BA2:
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
loc_007BAC:
	move.b	d2,(g_SprCountLo).w
	move.w	a1,(g_SATPtr).w
	rts
loc_007BB6:
	movea.l	(a6),a0
	movea.w	d0,a6
	add.w	a4,d4
	add.w	a5,d5
	movea.w	(a0)+,a2
	move.w	(a2),d0
	beq.s	loc_007BA2
	movea.w	d0,a2
	movea.l	(a0),a0
	move.w	(a0)+,d7
	btst	#$B,d6
	bne.w	loc_007C92
	btst	#$C,d6
	bne.s	loc_007C30
loc_007BD8:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	add.w	d5,d0
	cmpi.w	#$100,d0
	bcc.s	loc_007C20
	add.w	d4,d1
	cmpi.w	#$160,d1
	bcc.s	loc_007C20
	addi.w	#$60,d0
	addi.w	#$60,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007BD8
	beq.s	loc_007BAC
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.s	loc_007BB6
	bra.s	loc_007BAC
loc_007C20:
	dbra	d7,loc_007BD8
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.s	loc_007BB6
	bra.w	loc_007BAC
loc_007C30:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	swap	d0
	add.w	d5,d0
	cmpi.w	#$100,d0
	bcc.s	loc_007C80
	add.w	d4,d1
	cmpi.w	#$160,d1
	bcc.s	loc_007C80
	addi.w	#$60,d0
	addi.w	#$60,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007C30
	beq.w	loc_007BAC
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
	bra.w	loc_007BAC
loc_007C80:
	dbra	d7,loc_007C30
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
	bra.w	loc_007BAC
loc_007C92:
	btst	#$C,d6
	bne.s	loc_007CFA
loc_007C98:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	swap	d1
	add.w	d5,d0
	cmpi.w	#$100,d0
	bcc.s	loc_007CE8
	add.w	d4,d1
	cmpi.w	#$160,d1
	bcc.s	loc_007CE8
	addi.w	#$60,d0
	addi.w	#$60,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007C98
	beq.w	loc_007BAC
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
	bra.w	loc_007BAC
loc_007CE8:
	dbra	d7,loc_007C98
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
	bra.w	loc_007BAC
loc_007CFA:
	move.l	(a0)+,d0
	move.l	(a0)+,d1
	ext.w	d2
	or.w	(a0)+,d2
	move.b	(a0)+,d3
	move.b	(a0)+,d6
	swap	d0
	swap	d1
	add.w	d5,d0
	cmpi.w	#$100,d0
	bcc.s	loc_007D4C
	add.w	d4,d1
	cmpi.w	#$160,d1
	bcc.s	loc_007D4C
	addi.w	#$60,d0
	addi.w	#$60,d1
	addq.w	#$1,d2
	move.w	d0,(a1)+
	move.w	d2,(a1)+
	move.w	$6(a2,d3.w),d0
	add.w	d6,d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	cmpi.b	#$50,d2
	dbeq	d7,loc_007CFA
	beq.w	loc_007BAC
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
	bra.w	loc_007BAC
loc_007D4C:
	dbra	d7,loc_007CFA
	movem.w	(a6)+,d0/d4-d6
	addq.w	#$6,d0
	bcc.w	loc_007BB6
	bra.w	loc_007BAC
