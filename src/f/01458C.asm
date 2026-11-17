; ---------------------------------------------------------------------------
; Menu_Update - options-menu input: moves the cursor, then Left/Right step the selected setting through 0..its maximum with wrap-around
; In:  a0 = table: word last item, then 8 bytes per item (long setting word, long maximum word)
; Out: the setting word updated
; Evidence: bsr Menu_MoveCursor; entry cursor*8; bit 3 (right) +1 wraps to 0 past max, bit 2 (left) -1 wraps to max; caller $499FC with Tbl_049994 (options: music / sound test numbers $F60A/$F60C); scenario-options
; ---------------------------------------------------------------------------
Menu_Update:
	move.w	(a0)+,d1
	bsr.s	Menu_MoveCursor
	asl.w	#$3,d0
	adda.w	d0,a0
	movea.l	(a0)+,a1
	movea.l	(a0)+,a0
	move.w	(a1),d0
	btst	#$3,d2
	bne.s	loc_0145A8
	btst	#$2,d2
	bne.s	loc_0145B4
	rts
loc_0145A8:
	addq.w	#$1,d0
	cmp.w	(a0),d0
	ble.s	loc_0145B0
	moveq	#$0,d0
loc_0145B0:
	move.w	d0,(a1)
	rts
loc_0145B4:
	subq.w	#$1,d0
	bpl.s	loc_0145BA
	move.w	(a0),d0
loc_0145BA:
	move.w	d0,(a1)
	rts
