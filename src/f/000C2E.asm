; ---------------------------------------------------------------------------
; Op2_SetTimer0RandYield0 - a2 op for coroutine 0: sets timer 0 ($1A(a4)) to base + random(range) (Rand_Below), saves the next op in slot 0 ($38(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: words range, base; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer0; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer0RandYield0:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	move.w	d0,$1A(a4)
	move.l	a2,$38(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
