; ---------------------------------------------------------------------------
; Spr_AddSorted - adds a sprite frame to the display list in descending order of d7 (earlier = higher SAT priority = drawn in front)
; In:  a0 = frame-table entry (word art handle, long piece list), d4 = x, d5 = y, d6 = attribute word (bit 11/12 = H/V flip, high byte = palette/priority), d7 = depth key; a1/a2 preserved
; Out: an 18-byte key node, or a 12-byte entry chained to an existing node when the key is equal; nothing when the 80 entries are used up (g_SprNodesFree clamps at 0)
; Evidence: the walk from $9DE2 stops at the first node with key <= d7 (cmp.w (a2)+,d7 / bcc); the a2 VM sprite ops at $4524-$4630 call Spr_ChangeFrame then this with d7 = $3A(a5), d4/d5 = $3E/$42(a6)
; ---------------------------------------------------------------------------
Spr_AddSorted:
	subq.b	#$1,(g_SprNodesFree).w
	bcs.w	loc_007B16
	move.l	a1,d1
	move.l	a2,d2
	lea.l	($FFFF9DE2).w,a1
loc_007A5A:
	movea.w	(a1),a2
	cmp.w	(a2)+,d7
	bcc.s	loc_007AD0
	movea.w	(a2),a1
	cmp.w	(a1)+,d7
	bcc.s	loc_007A8A
	movea.w	(a1),a2
	cmp.w	(a2)+,d7
	bcc.s	loc_007AD0
	movea.w	(a2),a1
	cmp.w	(a1)+,d7
	bcc.s	loc_007A8A
	movea.w	(a1),a2
	cmp.w	(a2)+,d7
	bcc.s	loc_007AD0
	movea.w	(a2),a1
	cmp.w	(a1)+,d7
	bcc.s	loc_007A8A
	movea.w	(a1),a2
	cmp.w	(a2)+,d7
	bcc.s	loc_007AD0
	movea.w	(a2),a1
	cmp.w	(a1)+,d7
	bcs.s	loc_007A5A
loc_007A8A:
	beq.s	loc_007AB0
	move.w	a1,d0
	subq.w	#$2,d0
	movea.w	(g_SprNodeAlloc).w,a1
	move.l	a0,-(a1)
	movem.w	d0/d4-d6,-(a1)
	movem.w	d0/a1,-(a1)
	move.w	d7,-(a1)
	move.w	a1,(a2)+
	movea.w	(a2),a2
	move.w	a1,(a2)
	move.w	a1,(g_SprNodeAlloc).w
	movea.l	d1,a1
	movea.l	d2,a2
	rts
loc_007AB0:
	move.w	(a1)+,d0
	movea.w	(g_SprNodeAlloc).w,a2
	move.l	a0,-(a2)
	movem.w	d0/d4-d6,-(a2)
	move.w	a2,d0
	movea.w	(a1),a2
	move.w	d0,(a1)
	subq.w	#$6,d0
	move.w	d0,(a2)
	move.w	d0,(g_SprNodeAlloc).w
	movea.l	d1,a1
	movea.l	d2,a2
	rts
loc_007AD0:
	beq.s	loc_007AF6
	move.w	a2,d0
	subq.w	#$2,d0
	movea.w	(g_SprNodeAlloc).w,a2
	move.l	a0,-(a2)
	movem.w	d0/d4-d6,-(a2)
	movem.w	d0/a2,-(a2)
	move.w	d7,-(a2)
	move.w	a2,(a1)+
	movea.w	(a1),a1
	move.w	a2,(a1)
	move.w	a2,(g_SprNodeAlloc).w
	movea.l	d1,a1
	movea.l	d2,a2
	rts
loc_007AF6:
	move.w	(a2)+,d0
	movea.w	(g_SprNodeAlloc).w,a1
	move.l	a0,-(a1)
	movem.w	d0/d4-d6,-(a1)
	move.w	a1,d0
	movea.w	(a2),a1
	move.w	d0,(a2)
	subq.w	#$6,d0
	move.w	d0,(a1)
	move.w	d0,(g_SprNodeAlloc).w
	movea.l	d1,a1
	movea.l	d2,a2
	rts
loc_007B16:
	clr.b	(g_SprNodesFree).w
	rts
