; ---------------------------------------------------------------------------
; Layer_ScrollStepX - x half of Layer_ScrollStep: moves x by the pending +4 clamped to +/-16 and, on crossing a block edge, queues the block column entering at the left or right screen edge
; In:  a6 = layer
; Out: (a6) x, +4 reduced; Layer_QueueColumn (d0 = -1 left edge, or +1 with column/cell offsets $15/$2A for the right edge)
; Evidence: first call of Layer_ScrollStep; 57k TAS frames (391-101674)
; ---------------------------------------------------------------------------
Layer_ScrollStepX:
	move.w	$4(a6),d1
	beq.w	loc_012A72
	cmpi.w	#$10,d1
	bgt.s	loc_012A4A
	cmpi.w	#$FFF0,d1
	bgt.s	loc_012A4E
	move.w	#$FFF0,d1
	bra.s	loc_012A4E
loc_012A4A:
	move.w	#$10,d1
loc_012A4E:
	sub.w	d1,$4(a6)
	move.w	(a6),d0
	add.w	d1,d0
	move.w	d0,(a6)
	tst.w	d1
	bpl.s	loc_012A74
	not.w	d0
	andi.w	#$F,d0
	add.w	d1,d0
	bcs.w	loc_012A72
	moveq	#-$1,d0
	moveq	#$0,d1
	moveq	#-$2,d2
	moveq	#$0,d3
	bra.s	Layer_QueueColumn
loc_012A72:
	rts
loc_012A74:
	andi.w	#$F,d0
	sub.w	d1,d0
	bcc.s	loc_012A72
	moveq	#$1,d0
	moveq	#$15,d1
	moveq	#$2,d2
	moveq	#$2A,d3
