; ---------------------------------------------------------------------------
; Layer_WriteTiles - unrolled VDP writer for the tile queue: each 8-byte unit writes a control word then a data long; entered at the end minus the entry's count
; In:  a0/a1 = VDP control/data, a3 = queue entry after its count word, d0 = -(8 x count)
; Out: a3 past the entry; loops to the next entry until a 0 count
; Evidence: entered through Layer_WriteTilesStart (`jmp $12DB6(pc,d0.w)`); its tail tests the next count; executed in every corpus run with a map layer
; ---------------------------------------------------------------------------
Layer_WriteTiles:
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a1)
loc_012DB6:
	move.w	(a3)+,d0
	beq.s	Layer_WriteTilesEnd
; ---------------------------------------------------------------------------
; Layer_WriteTilesStart - writes an entry's VDP command high word and jumps into Layer_WriteTiles by the entry's count
; In:  a3 = entry (command), d0 = -(8 x count)
; Out: as Layer_WriteTiles
; Evidence: the bra target from Layer_FlushTileQueue and the loop of Layer_WriteTiles
; ---------------------------------------------------------------------------
Layer_WriteTilesStart:
	move.w	(a3)+,(a0)
	jmp	loc_012DB6(pc,d0.w)
; ---------------------------------------------------------------------------
; Layer_WriteTilesEnd - the rts that ends the tile-queue writer (zero count)
; In:  none
; Out: none
; Evidence: branched to from Layer_FlushTileQueue (empty queue) and Layer_WriteTiles
; ---------------------------------------------------------------------------
Layer_WriteTilesEnd:
	rts
; ---------------------------------------------------------------------------
; Trigger_LoadX - loads a stage's horizontal trigger table: insertion-sorts its entries by x key (+2) into g_TriggerList (offset words), sentinel $FFFF
; In:  a0 = table: word count, then entries (+0 size, +2 x key, +4 y key, +6/+8 x/y, +$A long callback whose top byte is a spawn action)
; Out: g_TriggerList filled, g_TriggerNext = its first entry, g_TriggerTable = a0 + 2
; Evidence: pointed to from 12 stage setups ($01FBA0, $033302, $040216, ...) with Tbl_015F5A, Tbl_0163BA, Tbl_0166EA...; the entries are the `dc.l Wave_StartFromEntry, script` lines NOTES describes; executed in 19 corpus runs
; ---------------------------------------------------------------------------
Trigger_LoadX:
	move.l	a0,d6
	lea.l	(g_TriggerList).l,a1
	clr.l	(a1)+
	move.l	a1,(g_TriggerNext).w
	move.w	(a0)+,d7
	move.l	a0,(g_TriggerTable).w
	move.l	a0,d6
	bra.s	loc_012DF6
loc_012DDA:
	movea.l	a1,a2
	lea.l	$4(a2),a3
	movea.l	a3,a1
	move.w	$2(a0),d0
loc_012DE6:
	move.l	-(a2),-(a3)
	cmp.w	(a2),d0
	bcs.s	loc_012DE6
	move.w	d0,(a3)+
	move.l	a0,d0
	sub.l	d6,d0
	move.w	d0,(a3)+
	adda.w	(a0),a0
loc_012DF6:
	dbra	d7,loc_012DDA
	move.l	#$FFFF0000,(a1)
	rts
