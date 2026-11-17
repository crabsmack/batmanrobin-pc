; ---------------------------------------------------------------------------
; Op5_LoopA - a5 op: counts loop counter A $1A(a6) down and branches back while it stays above 0; at 0 continues past the offset
; In:  a5 = script: word branch offset (negative: back to the loop body)
; Out: the branch (Op5_Goto) or NEXT
; Evidence: subq.w #1,$1A / bgt Op5_Goto / clr / addq #2; 147 script uses, e.g. $17572 (`6CB0 FFDA`); 57 corpus runs
; ---------------------------------------------------------------------------
Op5_LoopA:
	subq.w	#$1,$1A(a6)
	bgt.w	Op5_Goto
	clr.w	$1A(a6)
	addq.l	#$2,a5
	movea.w	(a5)+,a0
	jmp	(a0)
