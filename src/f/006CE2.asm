; ---------------------------------------------------------------------------
; Op5_SetLoopB - a5 op: sets loop counter B $1C(a6) to a word
; In:  a5 = script: word count
; Out: NEXT
; Evidence: move.w (a5)+,d0 -> $1C; script use $343AE (`6CE2 000A`); 3 corpus runs
; ---------------------------------------------------------------------------
Op5_SetLoopB:
	move.w	(a5)+,d0
; ---------------------------------------------------------------------------
; Op5_SetLoopB_Store - the shared store of the Op5_SetLoopB* ops: loop counter B = d0's low word, then NEXT
; In:  d0 = the count (moveq or the word operand); a5 = script; a6 = wave object
; Out: $1C(a6) = d0.w; NEXT
; Evidence: move.w d0,$1C(a6) / movea.w (a5)+,a0 / jmp (a0); bra.s here from Op5_SetLoopB2/B3 ($6CC6 file) and Op5_SetLoopB5 ($6CD2 file), so the recompiler made it a function
; ---------------------------------------------------------------------------
Op5_SetLoopB_Store:
	move.w	d0,$1C(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
