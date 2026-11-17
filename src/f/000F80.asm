; ---------------------------------------------------------------------------
; Op2_SetTimer1 - a2 op: sets timer 1 ($1C(a4)) from a word operand
; In:  a2 = script: word ticks (counted down by $30(a4) per run)
; Out: NEXT
; Evidence: counted down by Op2_TickTimer1Branch / Op2_LoopTimer1; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer1:
	move.w	(a2)+,$1C(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
