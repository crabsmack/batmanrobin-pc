; ---------------------------------------------------------------------------
; Op2_SetSprVar1 - a2 op: sets sprite-block variable 1 ($24(a5))
; In:  a2 = script: word value
; Out: NEXT
; Evidence: move.w (a2)+,$24(a5); same never-run op family follows ($40A0-$4128); $10512 steps $24(a5); 3 corpus runs; $18DC0 `4098 000C`
; ---------------------------------------------------------------------------
Op2_SetSprVar1:
	move.w	(a2)+,$24(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
