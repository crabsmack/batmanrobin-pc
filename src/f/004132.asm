; ---------------------------------------------------------------------------
; Op2_SetSprVar2 - a2 op: sets sprite-block variable 2 ($26(a5))
; In:  a2 = script: word value
; Out: NEXT
; Evidence: move.w (a2)+,$26(a5); $104BA returns $26(a5) to a native switch; 89 corpus runs; $4B6A2 `4132 0000`
; ---------------------------------------------------------------------------
Op2_SetSprVar2:
	move.w	(a2)+,$26(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
