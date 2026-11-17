; ---------------------------------------------------------------------------
; Op2_GosubAbsArgs - a2 op: calls an absolute script subroutine with four word arguments: stores them in the argument registers g_VM2Arg3, Arg2, Arg1, Arg0 (high words), pushes the return point and jumps
; In:  a2 = script: words arg3, arg2, arg1, arg0, long script address
; Out: (sp) = return a2 (after the long), a2 = subroutine; NEXT; Op2_Return comes back
; Evidence: four move.w (a2)+ to $DEDA/$DED6/$DED2/$DECE, pea 4(a2), movea.l (a2),a2; 6 runs; $2DFA6 `2CE6 FFF1 000F 00F0 00DC 00027D50`; its tail $2CF6 (gosub abs without arguments) has no script use found
; ---------------------------------------------------------------------------
Op2_GosubAbsArgs:
	move.w	(a2)+,(g_VM2Arg3).w
	move.w	(a2)+,(g_VM2Arg2).w
	move.w	(a2)+,(g_VM2Arg1).w
	move.w	(a2)+,(g_VM2Arg0).w
	pea.l	$4(a2)
	movea.l	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GosubArgs - a2 op: calls a relative script subroutine with four word arguments (g_VM2Arg3..Arg0 high words), then falls into Op2_Gosub
; In:  a2 = script: words arg3, arg2, arg1, arg0, word offset
; Out: (sp) = return a2, a2 = subroutine; NEXT
; Evidence: same stores as Op2_GosubAbsArgs, then pea 2(a2) / adda.w (a2),a2; 20 runs
; ---------------------------------------------------------------------------
Op2_GosubArgs:
	move.w	(a2)+,(g_VM2Arg3).w
; ---------------------------------------------------------------------------
; Op2_GosubArgs3 - a2 op (interior entry of Op2_GosubArgs): calls a relative script subroutine with three word arguments (g_VM2Arg2, Arg1, Arg0 high words; Arg3 kept)
; In:  a2 = script: words arg2, arg1, arg0, word offset
; Out: (sp) = return a2, a2 = subroutine; NEXT
; Evidence: a computed-jump target in the corpus: script handler words $2D04 dispatched from NEXT sites (Op2_Return's $2D2E 1,562, $3B3A 386, $0EA4 279, ...), 2,516 call-site log entries over the corpus
; ---------------------------------------------------------------------------
Op2_GosubArgs3:
	move.w	(a2)+,(g_VM2Arg2).w
; ---------------------------------------------------------------------------
; Op2_GosubArgs2 - a2 op (interior entry of Op2_GosubArgs): calls a relative script subroutine with two word arguments (g_VM2Arg1, Arg0 high words; Arg3/Arg2 kept)
; In:  a2 = script: words arg1, arg0, word offset
; Out: (sp) = return a2, a2 = subroutine; NEXT
; Evidence: a computed-jump target in the corpus: script handler words $2D08 dispatched from NEXT sites ($1BBC 240, ...), 277 call-site log entries over the corpus
; ---------------------------------------------------------------------------
Op2_GosubArgs2:
	move.w	(a2)+,(g_VM2Arg1).w
	move.w	(a2)+,(g_VM2Arg0).w
; ---------------------------------------------------------------------------
; Op2_Gosub -a2 op (interior entry of Op2_GosubArgs): calls a relative script subroutine: pushes the address after the offset and branches; Op2_Return pops it
; In:  a2 = script: word offset
; Out: (sp) = return a2, a2 = subroutine; NEXT
; Evidence: pea $2(a2) / adda.w (a2),a2; a computed-jump target ($2D10) in 146 of 153 runs; 391 script words $2D10, e.g. $18242 `2D10 FF08`
; ---------------------------------------------------------------------------
Op2_Gosub:
	pea.l	$2(a2)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_GotoAbs - a2 op: jumps to an absolute script address
; In:  a2 = script: long address
; Out: a2 = target; NEXT
; Evidence: movea.l (a2),a2; 12 runs; $2DF8E `2D1A 00027CD8`
; ---------------------------------------------------------------------------
Op2_GotoAbs:
	movea.l	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SkipGoto - a2 op tail: the branch-taken path that first skips one word (the ops with two operands before the offset), then Op2_Goto: a2 += the offset word, NEXT
; In:  a2 = script: a word to skip, then the branch offset
; Out: NEXT at the branch target
; Evidence: addq.l #2,a2 then falls into Op2_Goto; the blt/bgt targets of Op2_BranchOffScrX and Op2_SwitchScrX
; ---------------------------------------------------------------------------
Op2_SkipGoto:
	addq.l	#$2,a2
; ---------------------------------------------------------------------------
; Op2_Goto - a2 op and shared branch-taken tail: adds the word at a2 to a2 (offset relative to the offset word) and continues there; Op2_SkipGoto just before it skips one word first
; In:  a2 = script: word offset
; Out: a2 = target; NEXT
; Evidence: adda.w (a2),a2; dispatched in 152 runs (1586 script words $2D22) and branched to by most conditional ops
; ---------------------------------------------------------------------------
Op2_Goto:
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
