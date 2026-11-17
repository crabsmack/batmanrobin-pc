; ---------------------------------------------------------------------------
; Op2_SwitchVar0Bits - a2 op: bit switch on script variable 0 ($22(a4)): walks n (offset, mask) word pairs and branches by the offset of the first pair whose mask shares a bit with the variable; no match continues after the table
; In:  a2 = script: word n-1, then n longs (word offset relative to the end of its pair, word mask); a4 = object's a4 block
; Out: branch to the matched case, else NEXT after the table
; Evidence: move.l (a2)+,d1 / and.w d0,d1 / dbne d7 / swap d1 / adda.w d1,a2; 21 corpus runs incl. the TAS (12532 TAS frames)
; ---------------------------------------------------------------------------
Op2_SwitchVar0Bits:
	move.w	$22(a4),d0
	move.w	(a2)+,d7
loc_002094:
	move.l	(a2)+,d1
	and.w	d0,d1
	dbne	d7,loc_002094
	beq.s	loc_0020A2
	swap	d1
	adda.w	d1,a2
loc_0020A2:
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearVar1 - a2 op: clears the object's script variable 1 (word $24(a4), used as a flag word: bit $400 tested by the player code $10322)
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: clr.w $24(a4); the player spawn $101CE copies g_PlayMode into it; 36 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ClearVar1:
	clr.w	$24(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetVar1 - a2 op: sets script variable 1 ($24(a4)) to a word
; In:  a2 = script: word value; a4 = object's a4 block
; Out: NEXT
; Evidence: move.w (a2)+,$24(a4); 9 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetVar1:
	move.w	(a2)+,$24(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_OrVar1 - a2 op: sets bits in script variable 1 ($24(a4))
; In:  a2 = script: word bits; a4 = object's a4 block
; Out: NEXT
; Evidence: or.w; 135 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_OrVar1:
	move.w	(a2)+,d0
	or.w	d0,$24(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AndVar1 - a2 op: masks script variable 1 ($24(a4)) with a word (clears the bits not in it)
; In:  a2 = script: word mask; a4 = object's a4 block
; Out: NEXT
; Evidence: and.w; 144 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_AndVar1:
	move.w	(a2)+,d0
	and.w	d0,$24(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_XorVar1 - a2 op: toggles bits in script variable 1 ($24(a4))
; In:  a2 = script: word bits; a4 = object's a4 block
; Out: NEXT
; Evidence: eor.w; 20 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_XorVar1:
	move.w	(a2)+,d0
	eor.w	d0,$24(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_IncVar1 - a2 op: adds 1 to script variable 1 ($24(a4))
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: addq.w #1,$24(a4); 4 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_IncVar1:
	addq.w	#$1,$24(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
