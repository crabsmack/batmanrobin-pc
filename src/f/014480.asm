; ---------------------------------------------------------------------------
; TextMap_PutDigit - writes one decimal digit tile unless it and every digit before it were zero, then moves the division remainder down for the next divide
; In:  d0 = remainder:quotient from divu, d1 = OR of the digits so far, a5/d6/a6 as TextMap_Run
; Out: d0 = remainder (long), d1 updated, a6 advanced when written
; Evidence: or.w d0,d1 / beq skip; tile = $20(a5,digit*2) + d6 (font map entry of '0' + digit); clr.w d0 / swap d0
; ---------------------------------------------------------------------------
TextMap_PutDigit:
	or.w	d0,d1
	beq.s	loc_01448E
	add.w	d0,d0
	move.w	$20(a5,d0.w),d0
	add.w	d6,d0
	move.w	d0,(a6)+
loc_01448E:
	clr.w	d0
	swap	d0
	rts
