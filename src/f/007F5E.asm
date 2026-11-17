; ---------------------------------------------------------------------------
; Spr_SwapFrameArt - acquires VRAM art for a sprite's new frame (allocating blocks and queuing its DMA when not resident) and releases the old frame's art
; In:  a0 = new frame entry, d0 = its handle address, a1 = old frame entry, d1 = new frame (0 = none: only release), d2 = old frame, usp = caller's a1
; Out: a0 = new entry, d1 kept; on failure (DMA budget/slots, g_ArtBlocksFree or g_ArtDescsFree short) a0 = old entry and d1 = d2 so the old frame stays
; Evidence: tail of Spr_ChangeFrame (6 branches); art header at 6(a0): word cost, word block count; calls ArtCache_Alloc then Spr_QueueArtDMA; a resident handle just gets its refcount byte decremented (revived from cache when it was 0); runs on 74k TAS frames
; ---------------------------------------------------------------------------
Spr_SwapFrameArt:
	movem.l	d4-d7/a0-a4,-(a7)
	move.w	d1,-(a7)
	beq.s	loc_007F96
	movea.l	$6(a0),a4
	movea.w	d0,a0
	move.w	(a0),d0
	bne.s	loc_007FD2
	movem.w	(a4),d0-d1
	add.w	d1,d0
	cmp.w	(g_DMABudgetLeft).w,d0
	bgt.s	loc_007FF2
	cmp.w	(g_DMAQueueFree).w,d1
	bgt.s	loc_007FF2
	sub.b	d1,(g_ArtBlocksFree).w
	bcs.s	loc_007FEE
	subq.b	#$1,(g_ArtDescsFree).w
	bcs.s	loc_007FEA
	bsr.w	ArtCache_Alloc
	bsr.w	Spr_QueueArtDMA
loc_007F96:
	move.w	(a7)+,d1
	movem.l	(a7)+,d4-d7/a0-a4
	movea.w	(a1),a1
	move.w	(a1),d0
	beq.s	loc_007FC4
	movea.w	d0,a1
	addq.b	#$1,(a1)+
	bne.s	loc_007FC8
	addq.b	#$1,(g_ArtDescsFree).w
	move.b	(a1)+,d2
	add.b	d2,(g_ArtBlocksFree).w
	tst.w	(a1)
	bne.s	loc_007FC4
	move.w	(g_ArtRelRingWr).w,d2
	addq.b	#$2,(g_ArtRelRingWrLo).w
	move.w	d2,(a1)
	movea.w	d2,a1
	move.w	d0,(a1)
loc_007FC4:
	move.l	usp,a1
	rts
loc_007FC8:
	bmi.s	loc_007FC4
	clr.b	-$1(a1)
	move.l	usp,a1
	rts
loc_007FD2:
	movea.w	d0,a0
	subq.b	#$1,(a0)+
	bcc.s	loc_007FE4
	move.b	(a0),d0
	sub.b	d0,(g_ArtBlocksFree).w
	subq.b	#$1,(g_ArtDescsFree).w
	bra.s	loc_007F96
loc_007FE4:
	bmi.s	loc_007F96
	addq.b	#$1,(a0)
	bra.s	loc_007F96
loc_007FEA:
	clr.b	(g_ArtDescsFree).w
loc_007FEE:
	add.b	d1,(g_ArtBlocksFree).w
loc_007FF2:
	addq.l	#$2,a7
	movem.l	(a7)+,d4-d7/a0-a4
	movea.l	a1,a0
	move.w	d2,d1
	move.l	usp,a1
	rts
