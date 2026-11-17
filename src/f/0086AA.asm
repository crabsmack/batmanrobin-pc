; ---------------------------------------------------------------------------
; ArtCache_Release - drops one reference to the art behind handle a0; at the last one the blocks count as free and the descriptor is queued on the release ring
; In:  a0 = handle address
; Out: d0 = 0 released, 1 = still referenced, 2 = was not in use, -1 = handle empty
; Evidence: callers Spr_InitBankFrames and $11D4E; same refcount convention as Spr_SwapFrameArt (negative byte = users, 0 = cached and evictable)
; ---------------------------------------------------------------------------
ArtCache_Release:
	move.w	(a0),d0
	beq.s	loc_0086E2
	movea.w	d0,a0
	addq.b	#$1,(a0)+
	bne.s	loc_0086D4
	move.b	(a0)+,d1
	add.b	d1,(g_ArtBlocksFree).w
	tst.w	(a0)
	bne.s	loc_0086CC
	move.w	(g_ArtRelRingWr).w,d1
	addq.b	#$2,(g_ArtRelRingWrLo).w
	move.w	d1,(a0)
	movea.w	d1,a0
	move.w	d0,(a0)
loc_0086CC:
	addq.b	#$1,(g_ArtDescsFree).w
	moveq	#$0,d0
	rts
loc_0086D4:
	bmi.s	loc_0086DE
	clr.b	-$1(a0)
	moveq	#$2,d0
	rts
loc_0086DE:
	moveq	#$1,d0
	rts
loc_0086E2:
	moveq	#-$1,d0
	rts
