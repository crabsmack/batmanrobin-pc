; ---------------------------------------------------------------------------
; Op2_SkipWord - a2 op / shared tail: skips one script word (a branch offset that is not taken) and continues (NEXT)
; In:  a2 = script: word ignored
; Out: NEXT at a2+2
; Evidence: the not-taken path of the conditional branches Op2_TickTimer0Branch-style ops ($EC0, $F9A, $1074) and $146C; runs on 86k TAS frames
; ---------------------------------------------------------------------------
Op2_SkipWord:
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
