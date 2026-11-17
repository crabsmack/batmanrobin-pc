; ---------------------------------------------------------------------------
; Op5_SetLoopB5 - a5 op: sets loop counter B $1C(a6) to 5 (the 4 and 6-8 entries around it never ran and stay raw)
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #5 -> $1C; script use $21F74 (`6CD2 6C62`); 1 corpus run
; ---------------------------------------------------------------------------
Op5_SetLoopB5:
	moveq	#$5,d0
	bra.s	Op5_SetLoopB_Store
