; ---------------------------------------------------------------------------
; Span_ClearDrawDisc - clears all 224 span lines, then draws the dithered disc alone (Span_DrawDiscDither)
; In:  as Span_DrawDiscDither
; Out: as Span_DrawDiscDither
; Evidence: Span_ClearLines with d7 = $E0, d0 = d2 = 0 under movem, then jmp Span_DrawDiscDither; jmp from $334D2; TAS frames 79014-79108
; ---------------------------------------------------------------------------
Span_ClearDrawDisc:
	movem.l	d0-d2/a0-a1,-(a7)
	move.w	#$E0,d7
	moveq	#$0,d0
	moveq	#$0,d2
	jsr	Span_ClearLines(pc)
	movem.l	(a7)+,d0-d2/a0-a1
	jmp	Span_DrawDiscDither(pc)
