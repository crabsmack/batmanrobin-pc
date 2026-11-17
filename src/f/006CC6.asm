; ---------------------------------------------------------------------------
; Op5_SetLoopB2 - a5 op: sets loop counter B $1C(a6) to 2
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #2 -> $1C; 65 script uses, e.g. $22C80, $21E82; 5 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopB2:
	moveq	#$2,d0
	bra.s	Op5_SetLoopB_Store
; ---------------------------------------------------------------------------
; Op5_SetLoopB3 - a5 op: sets loop counter B $1C(a6) to 3
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #3 -> $1C; script uses $22504, $22BA0; 5 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopB3:
	moveq	#$3,d0
	bra.s	Op5_SetLoopB_Store
