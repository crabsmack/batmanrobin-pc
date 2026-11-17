; ---------------------------------------------------------------------------
; Op2_SetTimer3 - a2 op: sets timer 3 ($20(a4)) from a word operand
; In:  a2 = script: word ticks (counted down by $30(a4) per run)
; Out: NEXT
; Evidence: counted down by Op2_TickTimer3Branch / Op2_LoopTimer3; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer3:
	move.w	(a2)+,$20(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer3Rand - a2 op: sets timer 3 ($20(a4)) to base + random(range) (Rand_Below)
; In:  a2 = script: words range, base
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer3Rand:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	move.w	d0,$20(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
