; ---------------------------------------------------------------------------
; Op2_ClearNewBlock - shared tail of Op2_AllocSubBlock: clears 64 bytes (16 longs) at a0 and continues the script; not dispatched as an op on its own
; In:  a0 = block, a2 = script
; Out: NEXT
; Evidence: both paths of Op2_AllocSubBlock bra here; the file also holds Op2_ResetA3 ($1266); 20 TAS frames
; ---------------------------------------------------------------------------
Op2_ClearNewBlock:
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
; ---------------------------------------------------------------------------
; Op2_ResetA3 - a2 op: restores the motion block a3 to the object's own first block handle $8(a6)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: undoes Op2_LoadA3Handle; executed in the TAS
; ---------------------------------------------------------------------------
Op2_ResetA3:
	movea.w	$8(a6),a3
	movea.w	(a2)+,a0
	jmp	(a0)
