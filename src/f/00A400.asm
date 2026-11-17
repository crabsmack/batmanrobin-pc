; ---------------------------------------------------------------------------
; Pad_ReadPort - reads one 3-button pad port with the TH toggle and returns its buttons active-high, dropping opposite directions pressed together
; In:  a4 = pad data port ($A10003 or $A10005)
; Out: d0 = buttons: bit 7 Start, 6 A, 5 C, 4 B, 3 right, 2 left, 1 down, 0 up (up+down both pressed -> both cleared, left+right likewise); d1 trashed; TH left high
; Evidence: TH=0 read (bits 4/5 = A/Start) shifted left 2 into bits 6/7, TH=1 read gives C B R L D U in bits 5-0, then not.b; eor of the pair when both bits are set; only caller Pad_Update; every TAS frame from 18
; ---------------------------------------------------------------------------
Pad_ReadPort:
	move.b	#$0,(a4)
	nop
	nop
	move.b	(a4),d0
	lsl.b	#$2,d0
	move.b	#$40,(a4)
	andi.b	#$C0,d0
	move.b	(a4),d1
	andi.b	#$3F,d1
	or.b	d1,d0
	not.b	d0
	move.b	d0,d1
	andi.b	#$3,d1
	cmpi.b	#$3,d1
	bne.s	loc_00A42C
	eor.b	d1,d0
loc_00A42C:
	move.b	d0,d1
	andi.b	#$C,d1
	cmpi.b	#$C,d1
	bne.s	loc_00A43A
	eor.b	d1,d0
loc_00A43A:
	rts
