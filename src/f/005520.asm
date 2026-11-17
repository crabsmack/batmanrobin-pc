; ---------------------------------------------------------------------------
; Op4_BranchScreenYOutside - a4 op: compares the drawn y d5 minus g_CameraY with [lo, hi] and continues past the operands when inside; outside it branches
; In:  a4 = script: word lo, word hi, word offset; d5 = drawn y (low word, from the motion op)
; Out: above hi: Op4_Goto by the offset word; below lo: Op4_Goto by the hi word read as an offset (the cmp already advanced a4: a script bug or never hit, inferred); inside: NEXT
; Evidence: cmp.w (a4)+ / blt Op4_Goto / cmp.w (a4)+ / bgt Op4_Goto / addq.l #2,a4; uses at $3CE6C, $3CED2 (lo -$64, hi $94, offset $A) after the move op $632E; 3 corpus runs, 2456 TAS frames
; ---------------------------------------------------------------------------
Op4_BranchScreenYOutside:
	move.w	d5,d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a4)+,d0
	blt.w	Op4_Goto
	cmp.w	(a4)+,d0
	bgt.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
