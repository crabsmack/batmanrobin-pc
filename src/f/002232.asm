; ---------------------------------------------------------------------------
; Op2_SwitchVar2Bits - a2 op: the bit switch of Op2_SwitchVar0Bits on script variable 2 ($26(a4))
; In:  a2 = script: word n-1, then n longs (word offset, word mask); a4 = object's a4 block
; Out: branch to the first matching case, else NEXT after the table
; Evidence: same loop as $208E with $26(a4); 21 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SwitchVar2Bits:
	move.w	$26(a4),d0
	move.w	(a2)+,d7
loc_002238:
	move.l	(a2)+,d1
	and.w	d0,d1
	dbne	d7,loc_002238
	beq.s	loc_002246
	swap	d1
	adda.w	d1,a2
loc_002246:
	movea.w	(a2)+,a0
	jmp	(a0)
