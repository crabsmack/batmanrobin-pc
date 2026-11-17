; ---------------------------------------------------------------------------
; Span_ClearLines - fills d7 lines of the span tables with 'no span': H-scroll d0 and a V-scroll that counts down one per line, so every line keeps showing the same plane row (row 0 when d2 = -line)
; In:  a0 = H-scroll line table, a1 = V-scroll line table (g_HIntTblBack), d0.w = H-scroll, d2.w = V-scroll of the first line, d7.w = lines (0-224)
; Out: a0/a1 past the lines, d2 decremented once per line
; Evidence: `d7*6` computed jmp into Span_ClearLinesRun; callers always pass d0 = 0 and d2 = 0 or -(lines already done): Span_ClearScreen(Alt), the triangle and disc code before/after the shape, $40980; 12 corpus runs, TAS frames 77185-128793
; ---------------------------------------------------------------------------
Span_ClearLines:
	add.w	d7,d7
	move.w	d7,-(a7)
	add.w	d7,d7
	add.w	(a7)+,d7
	neg.w	d7
	jmp	loc_00D81C(pc,d7.w)
