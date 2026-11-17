; ---------------------------------------------------------------------------
; Spr_AddBack - appends a sprite frame after the last entry of the sorted display list, so it is drawn behind everything added so far
; In:  a0 = frame-table entry, d4 = x, d5 = y, d6 = attribute word; a1 preserved
; Out: the old tail sentinel slot ($9DF6) holds the entry and a new sentinel (next = $FFFF) is carved from the node pool; nothing when g_SprNodesFree runs out
; Evidence: writes below the tail pointer $9DF6 (initialised to $9E04 by Spr_ResetList) and moves the $FFFF end marker to the new node; callers $478C, $4AB4, $6776, $20A48, $36CD4
; ---------------------------------------------------------------------------
Spr_AddBack:
	subq.b	#$1,(g_SprNodesFree).w
	bmi.s	loc_007B4A
	move.l	a1,d1
	movea.w	(g_SprSortTail).w,a1
	move.w	(g_SprNodeAlloc).w,d0
	move.w	d0,(g_SprSortTail).w
	subi.w	#$12,d0
	move.l	a0,-(a1)
	movem.w	d0/d4-d6,-(a1)
	movea.w	d0,a1
	move.w	#$FFFF,$6(a1)
	move.w	d0,(g_SprNodeAlloc).w
	movea.l	d1,a1
	rts
loc_007B4A:
	clr.b	(g_SprNodesFree).w
	rts
