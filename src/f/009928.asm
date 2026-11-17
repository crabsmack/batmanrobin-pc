; ---------------------------------------------------------------------------
; Scroll_FillBands - fills 224 screen lines of a line H-scroll buffer from a band table: bands above the view are skipped, each visible band's lines get -(x / divisor)
; In:  a0 = line buffer, a1 = band table (word height, word divisor ... height 0 ends; $7FFF = rest of the screen), d2 = x (16.8, so divisor $100 = 1:1), d3 = view y
; Out: a0 past the lines written (Mem_FillWords fills d7 words with d0)
; Evidence: d6 = $E0 lines left; divs.w (a1)+ on d2; callers ScrollCmd_CameraA/CameraB/CameraOfsB and Scroll_BandsFromLayer; tables $971C ($7FFF,$100), $9720 ($60,$280 / $20,$180 / $7FFF,$C0); TAS f6362-f119385
; ---------------------------------------------------------------------------
Scroll_FillBands:
	move.w	#$E0,d6
	move.w	d3,d7
	neg.w	d7
	add.w	(a1)+,d7
	bgt.s	loc_00993A
loc_009934:
	addq.l	#$2,a1
	add.w	(a1)+,d7
	ble.s	loc_009934
loc_00993A:
	sub.w	d7,d6
	bpl.s	loc_009940
loc_00993E:
	add.w	d6,d7
loc_009940:
	move.l	d2,d0
	divs.w	(a1)+,d0
	neg.w	d0
	bsr.w	Mem_FillWords
	tst.w	d6
	ble.s	loc_009958
	move.w	(a1)+,d7
	beq.s	loc_009958
	sub.w	d7,d6
	bpl.s	loc_009940
	bra.s	loc_00993E
loc_009958:
	rts
