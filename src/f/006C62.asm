; ---------------------------------------------------------------------------
; Op5_SetLoopA0 - a5 op: sets loop counter A $1A(a6) to 0 (an Op5_LoopA then falls through at once)
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #0 -> $1A; script uses $21E08, $21E18; 19 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA0:
	moveq	#$0,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA2 - a5 op: sets loop counter A $1A(a6) to 2 (the body before Op5_LoopA runs twice)
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #2 -> $1A; 89 script uses, e.g. $1754C (`6C66 6DD4`); 16 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA2:
	moveq	#$2,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA3 - a5 op: sets loop counter A $1A(a6) to 3
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #3 -> $1A; script uses $21E5E, $22CBA; 5 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA3:
	moveq	#$3,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA4 - a5 op: sets loop counter A $1A(a6) to 4
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #4 -> $1A; script uses $21FDA, $2220E; 9 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA4:
	moveq	#$4,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA5 - a5 op: sets loop counter A $1A(a6) to 5
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #5 -> $1A; script uses $21EA6, $21F8A; 9 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA5:
	moveq	#$5,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA6 - a5 op: sets loop counter A $1A(a6) to 6
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #6 -> $1A; script uses $23DC0, $23E9C; 15 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA6:
	moveq	#$6,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA7 - a5 op: sets loop counter A $1A(a6) to 7
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #7 -> $1A; script uses $3C4AC, $3C4CC; 3 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA7:
	moveq	#$7,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA8 - a5 op: sets loop counter A $1A(a6) to 8
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: moveq #8 -> $1A; script uses $3C5B8, $3C5D0; 3 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA8:
	moveq	#$8,d0
	bra.s	loc_006C84
; ---------------------------------------------------------------------------
; Op5_SetLoopA - a5 op: sets loop counter A $1A(a6) to a word
; In:  a5 = script: word count
; Out: NEXT
; Evidence: move.w (a5)+,d0 -> $1A; script uses $21762 (`6C82 000C`), $23D02; 8 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopA:
	move.w	(a5)+,d0
loc_006C84:
	move.w	d0,$1A(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
