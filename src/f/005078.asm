; ---------------------------------------------------------------------------
; Op2_ClearMotionBlock - a2 op: zeroes the 64-byte block whose address is at $8(a6) (the motion block a3 is reset to by Op2_ResetA3), then NEXT
; In:  a6 = object, a2 = script (no operands)
; Out: the block cleared; continues with the next op
; Evidence: never run in the corpus; in the a2 handler range between Op2_AllocOsc and Op2_StopOsc, 16 x move.l d0,(a0)+ then movea.w (a2)+,a0 / jmp (a0); was labelled Scr2_005078 by datamap's script-word rule (its words happen to be handler addresses). Purpose inferred from the code
; ---------------------------------------------------------------------------
Op2_ClearMotionBlock:
	movea.w	$8(a6),a0
	moveq	#$0,d0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	movea.w	(a2)+,a0
	jmp	(a0)
