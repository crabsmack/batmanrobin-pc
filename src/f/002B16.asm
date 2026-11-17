; ---------------------------------------------------------------------------
; Op2_StartChildCoroutine1Rel - a2 op: starts the child object's coroutine slot 1 ($3C of its a4) at a relative address in the parent's script
; In:  a2 = script: word a6 field offset (child), word offset
; Out: NEXT
; Evidence: bsr Child_GetA4Skip2 / move.l a1,$3C(a0); 10 runs
; ---------------------------------------------------------------------------
Op2_StartChildCoroutine1Rel:
	bsr.w	Child_GetA4Skip2
	movea.l	a2,a1
	adda.w	(a2)+,a1
	move.l	a1,$3C(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
