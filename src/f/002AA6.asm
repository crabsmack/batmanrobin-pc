; ---------------------------------------------------------------------------
; Op2_StartChildCoroutine0Rel - a2 op: starts the child object's coroutine slot 0 ($38 of its a4) at a relative script address in the parent's script; does nothing without a child
; In:  a2 = script: word a6 field offset (child), word offset
; Out: NEXT
; Evidence: bsr Child_GetA4Skip2 / adda.w (a2)+ / move.l a1,$38(a0); 23 runs; $39686 `244A 002A ... 2AA6 002A`
; ---------------------------------------------------------------------------
Op2_StartChildCoroutine0Rel:
	bsr.w	Child_GetA4Skip2
	movea.l	a2,a1
	adda.w	(a2)+,a1
	move.l	a1,$38(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
