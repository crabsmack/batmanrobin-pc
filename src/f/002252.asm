; ---------------------------------------------------------------------------
; Op2_SetVar3 - a2 op: sets script variable 3 (word $28(a4)) to a word
; In:  a2 = script: word value; a4 = object's a4 block
; Out: NEXT
; Evidence: move.w (a2)+,$28(a4); native users $25D6E/$25D80 treat it as a step added to $26; 1 corpus run (the TAS, frame 33605)
; ---------------------------------------------------------------------------
Op2_SetVar3:
	move.w	(a2)+,$28(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_OrVar3 - a2 op: sets bits in script variable 3 ($28(a4))
; In:  a2 = script: word bits; a4 = object's a4 block
; Out: NEXT
; Evidence: or.w; 11 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_OrVar3:
	move.w	(a2)+,d0
	or.w	d0,$28(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AndVar3 - a2 op: masks script variable 3 ($28(a4)) with a word
; In:  a2 = script: word mask; a4 = object's a4 block
; Out: NEXT
; Evidence: and.w; 10 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_AndVar3:
	move.w	(a2)+,d0
	and.w	d0,$28(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
