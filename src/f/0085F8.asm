; ---------------------------------------------------------------------------
; ArtCache_Alloc - gives an art handle a descriptor and d1 VRAM blocks, evicting released art from the release ring when the free ring runs short
; In:  a0 = handle address, d1 = blocks; the caller has already reserved g_ArtBlocksFree/g_ArtDescsFree
; Out: (a0) = descriptor (refcount -1, block count, ring flag 0, back pointer = a0, block tile list); a3 = its block list
; Evidence: callers Spr_SwapFrameArt, ArtCache_Acquire, $6776. Evicted descriptors (refcount 0 on the release ring) lose their handle (clr.w via the back pointer) and go back to the descriptor ring; surplus blocks go back to the free ring; ring pointers live at $A766-$A76E and advance by addq.b #2 (256-byte rings)
; ---------------------------------------------------------------------------
ArtCache_Alloc:
	movem.w	(g_ArtRelRingRd).w,d2-d7
	subq.b	#$1,(g_ArtDescsSpare).w
	bcs.s	loc_00867A
	movea.l	d4,a2
	addq.b	#$2,d4
	movea.w	(a2),a2
	move.w	a2,(a0)
	subq.b	#$1,(a2)+
	move.b	d1,(a2)+
	clr.w	(a2)+
	move.w	a0,(a2)+
	movea.l	a2,a3
loc_008616:
	cmp.b	d7,d1
	bgt.s	loc_008634
	sub.b	d1,d7
	subq.b	#$1,d1
loc_00861E:
	movea.l	d6,a0
	addq.b	#$2,d6
	move.w	(a0),(a2)+
	dbra	d1,loc_00861E
	movem.w	d2-d7,(g_ArtRelRingRd).w
	rts
loc_008630:
	clr.w	$2(a0)
loc_008634:
	movea.l	d2,a0
	addq.b	#$2,d2
	movea.w	(a0),a0
	move.w	(a0),d0
	ble.s	loc_008630
	addq.b	#$1,(g_ArtDescsSpare).w
	movea.l	d3,a1
	addq.b	#$2,d3
	move.w	a0,(a1)
	addq.l	#$4,a0
	movea.w	(a0)+,a1
	clr.w	(a1)
loc_00864E:
	subq.w	#$1,d0
loc_008650:
	move.w	(a0)+,(a2)+
	subq.b	#$1,d1
	dbeq	d0,loc_008650
	bne.s	loc_008616
	add.b	d0,d7
	dbra	d0,loc_008668
	movem.w	d2-d7,(g_ArtRelRingRd).w
	rts
loc_008668:
	movea.l	d5,a1
	addq.b	#$2,d5
	move.w	(a0)+,(a1)
	dbra	d0,loc_008668
	movem.w	d2-d7,(g_ArtRelRingRd).w
	rts
loc_00867A:
	clr.b	(g_ArtDescsSpare).w
	movea.w	d2,a2
	addq.b	#$2,d2
	movea.w	(a2),a2
	move.w	(a2),d0
	bpl.s	loc_008696
loc_008688:
	clr.w	$2(a2)
	movea.w	d2,a2
	addq.b	#$2,d2
	movea.w	(a2),a2
	move.w	(a2),d0
	bmi.s	loc_008688
loc_008696:
	move.w	a2,(a0)
	subq.b	#$1,(a2)+
	move.b	d1,(a2)+
	clr.w	(a2)+
	movea.w	(a2),a1
	clr.w	(a1)
	move.w	a0,(a2)+
	movea.l	a2,a3
	movea.l	a2,a0
	bra.s	loc_00864E
