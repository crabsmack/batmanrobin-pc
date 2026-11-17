; ---------------------------------------------------------------------------
; Op2_StartChildCoroutine2Rel - a2 op: starts the child object's coroutine slot 2 ($40 of its a4) at a relative address in the parent's script
; In:  a2 = script: word a6 field offset (child), word offset
; Out: NEXT
; Evidence: bsr Child_GetA4Skip2 / move.l a1,$40(a0); 1 run (TAS f92319-93438)
; ---------------------------------------------------------------------------
Op2_StartChildCoroutine2Rel:
	bsr.w	Child_GetA4Skip2
	movea.l	a2,a1
	adda.w	(a2)+,a1
	move.l	a1,$40(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
