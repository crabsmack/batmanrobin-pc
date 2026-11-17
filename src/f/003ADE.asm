; ---------------------------------------------------------------------------
; Op2_SetVector1HereGoto - a2 op: points script vector 1 ($36(a6)) at the op right after its offset word and continues at the offset target
; In:  a2 = script: word offset
; Out: $36(a6) = address after the operand; NEXT at the target
; Evidence: same code as $3AC2 on $36(a6); the never-run bytes $3ACE-$3ADD before it set g_VM2Arg3..0 and fall in; 105 corpus runs, 376 script uses (e.g. $18264 `3ADE 14F6`)
; ---------------------------------------------------------------------------
Op2_SetVector1HereGoto:
	movea.l	a2,a0
	adda.w	(a0)+,a2
	move.l	a0,$36(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetVector2HereGotoArgs4 - a2 op: stores four words in g_VM2Arg3..g_VM2Arg0, then points script vector 2 ($3A(a6)) at the op after the offset word and jumps to the offset
; In:  a2 = script: words arg3, arg2, arg1, arg0, word offset
; Out: g_VM2Arg3..0 high words, $3A(a6) = address after the offset; NEXT at the target
; Evidence: four move.w (a2)+ into $DEDA/$DED6/$DED2/$DECE, then the vector-2 tail $3AFA; 33 corpus runs; $2A030 `3AEA 0014 FFEC 00D2 00B4 DCFA`
; ---------------------------------------------------------------------------
Op2_SetVector2HereGotoArgs4:
	move.w	(a2)+,(g_VM2Arg3).w
	move.w	(a2)+,(g_VM2Arg2).w
; ---------------------------------------------------------------------------
; Op2_SetVector2HereGotoArgs2 - a2 op: interior entry of Op2_SetVector2HereGotoArgs4 that stores only g_VM2Arg1 and g_VM2Arg0 before setting vector 2 and jumping
; In:  a2 = script: words arg1, arg0, word offset
; Out: as Op2_SetVector2HereGotoArgs4
; Evidence: dispatched at $3AF2 (37 corpus runs; 552 script word matches, e.g. $288DC)
; ---------------------------------------------------------------------------
Op2_SetVector2HereGotoArgs2:
	move.w	(a2)+,(g_VM2Arg1).w
; ---------------------------------------------------------------------------
; Op2_SetVector2HereGotoArg - a2 op: interior entry of Op2_SetVector2HereGotoArgs4 that stores only g_VM2Arg0 before setting vector 2 and jumping
; In:  a2 = script: word arg0, word offset
; Out: as Op2_SetVector2HereGotoArgs4
; Evidence: dispatched at $3AF6 (3 corpus runs); $18580 `3AF6 000C 1276`
; ---------------------------------------------------------------------------
Op2_SetVector2HereGotoArg:
	move.w	(a2)+,(g_VM2Arg0).w
; ---------------------------------------------------------------------------
; Op2_SetVector2HereGoto - a2 op: points script vector 2 ($3A(a6)) at the op right after its offset word and continues at the offset target (no arguments)
; In:  a2 = script: word offset
; Out: $3A(a6) = address after the operand; NEXT at the target
; Evidence: dispatched at $3AFA (101 corpus runs; 665 script uses, e.g. $18530 `3AFA 1254`); Op2_GotoVector2/GosubVector2 return through it
; ---------------------------------------------------------------------------
Op2_SetVector2HereGoto:
	movea.l	a2,a0
	adda.w	(a0)+,a2
	move.l	a0,$3A(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GotoVector0 - a2 op: continues the script at script vector 0 ($32(a6))
; In:  a2 = script (no operand); $32(a6) set by Op2_SetVector0/Op2_SetVector0HereGoto
; Out: NEXT at the vector
; Evidence: movea.l $32(a6),a2; 83 corpus runs; $1823C `3B06 3B0E 3B16`
; ---------------------------------------------------------------------------
Op2_GotoVector0:
	movea.l	$32(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GotoVector1 - a2 op: continues the script at script vector 1 ($36(a6))
; In:  a2 = script (no operand); $36(a6) set by Op2_SetVector1/Op2_SetVector1HereGoto
; Out: NEXT at the vector
; Evidence: movea.l $36(a6),a2; 75 corpus runs; $198C8 `3B0E 433E 0017`
; ---------------------------------------------------------------------------
Op2_GotoVector1:
	movea.l	$36(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GotoVector2 - a2 op: continues the script at script vector 2 ($3A(a6))
; In:  a2 = script (no operand); $3A(a6) set by the Op2_SetVector2HereGoto family
; Out: NEXT at the vector
; Evidence: movea.l $3A(a6),a2; 92 corpus runs; $1981C `3B16`
; ---------------------------------------------------------------------------
Op2_GotoVector2:
	movea.l	$3A(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GosubVector0 - a2 op: calls the script at vector 0 ($32(a6)) as a subroutine, pushing the next op for Op2_Return
; In:  a2 = script (no operand)
; Out: (sp) = return a2; NEXT at the vector
; Evidence: pea.l (a2) / movea.l $32(a6),a2; Op2_Return ($2D2A) pops a2; 42 corpus runs; $27CA8 `3B1E 1008 0004 3B28 2D2A`
; ---------------------------------------------------------------------------
Op2_GosubVector0:
	pea.l	(a2)
	movea.l	$32(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GosubVector1 - a2 op: calls the script at vector 1 ($36(a6)) as a subroutine, pushing the next op for Op2_Return
; In:  a2 = script (no operand)
; Out: (sp) = return a2; NEXT at the vector
; Evidence: pea.l (a2) / movea.l $36(a6),a2; 40 corpus runs; $13FE6 `3B28 2D22 FFDA`
; ---------------------------------------------------------------------------
Op2_GosubVector1:
	pea.l	(a2)
	movea.l	$36(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GosubVector2 - a2 op: calls the script at vector 2 ($3A(a6)) as a subroutine, pushing the next op for Op2_Return
; In:  a2 = script (no operand)
; Out: (sp) = return a2; NEXT at the vector
; Evidence: pea.l (a2) / movea.l $3A(a6),a2; 15 corpus runs; $286CC `3B32 3EB8 0574`
; ---------------------------------------------------------------------------
Op2_GosubVector2:
	pea.l	(a2)
	movea.l	$3A(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
