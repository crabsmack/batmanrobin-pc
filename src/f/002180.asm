; ---------------------------------------------------------------------------
; Op2_SetVar2 - a2 op: sets script variable 2 (word $26(a4)) to a word
; In:  a2 = script: word value; a4 = object's a4 block
; Out: NEXT
; Evidence: move.w (a2)+,$26(a4); native users $1A04C, $25D80 (adds $28 to it), $26656 (as a y); 2 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetVar2:
	move.w	(a2)+,$26(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_OrVar2 - a2 op: sets bits in script variable 2 ($26(a4))
; In:  a2 = script: word bits; a4 = object's a4 block
; Out: NEXT
; Evidence: or.w; 1 corpus run (the TAS)
; ---------------------------------------------------------------------------
Op2_OrVar2:
	move.w	(a2)+,d0
	or.w	d0,$26(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
