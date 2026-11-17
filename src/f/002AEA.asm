; ---------------------------------------------------------------------------
; Op2_StopChildCoroutine1 - a2 op: stops the child object's coroutine slot 1 (sets its $3C(a4) to Scr2_002D28)
; In:  a2 = script: word a6 field offset (child)
; Out: NEXT
; Evidence: bsr Child_GetA4 / move.l #$2D28,$3C(a0); 10 runs; $31CF0 `2AEA 002A` before `2B16 002A`
; ---------------------------------------------------------------------------
Op2_StopChildCoroutine1:
	bsr.w	Child_GetA4
	move.l	#$2D28,$3C(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
