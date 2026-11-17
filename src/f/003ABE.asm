; ---------------------------------------------------------------------------
; Op2_SetVector0HereGotoArg - a2 op: stores a word in g_VM2Arg0, then as Op2_SetVector0HereGoto points script vector 0 ($32(a6)) at the op after the offset word and jumps to the offset
; In:  a2 = script: word arg (high word of g_VM2Arg0), word offset
; Out: $32(a6) = address after the offset; NEXT at the offset target
; Evidence: falls into $3AC2; 8 corpus runs; 25 script uses, e.g. $3932C `3ABE FD00 E340`
; ---------------------------------------------------------------------------
Op2_SetVector0HereGotoArg:
	move.w	(a2)+,(g_VM2Arg0).w
; ---------------------------------------------------------------------------
; Op2_SetVector0HereGoto - a2 op: points script vector 0 ($32(a6)) at the op right after its offset word and continues at the offset target, so Op2_GotoVector0/GosubVector0 later come back here (a jump that leaves a return point)
; In:  a2 = script: word offset (from the operand)
; Out: $32(a6) = address after the operand; NEXT at the target
; Evidence: movea.l a2,a0 / adda.w (a0)+,a2 / move.l a0,$32(a6); interior entry of Op2_SetVector0HereGotoArg; 115 corpus runs, 801 script uses (e.g. $18246 `3AC2 0014`)
; ---------------------------------------------------------------------------
Op2_SetVector0HereGoto:
	movea.l	a2,a0
	adda.w	(a0)+,a2
	move.l	a0,$32(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
