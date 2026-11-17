; ---------------------------------------------------------------------------
; Span_BuildCircle - builds the half-width table of a circle of radius d0 for Span_DrawDiscDither: r+1 words at g_CircleSpans, from the top row (0) to the centre row (r), by the midpoint algorithm
; In:  d0.w = radius (sign ignored)
; Out: g_CircleSpans[0..r]; d0-d4, a0/a1 used
; Evidence: x counts up into (a0)+ from $E606 while y counts down into -(a1) from $E606+2r+2, error term stepped by 4; callers $334D2 (twice, radius $E5F6) before Span_DrawDiscDither / Span_ClearDrawDisc; TAS frames 77312-79108
; ---------------------------------------------------------------------------
Span_BuildCircle:
	tst.w	d0
	bgt.s	loc_010128
	neg.w	d0
loc_010128:
	move.w	d0,d4
	add.w	d0,d0
	lea.l	(g_CircleSpans).l,a0
	lea.l	$2(a0,d0.w),a1
	moveq	#$0,d0
	move.w	d4,d1
	moveq	#$0,d2
	moveq	#-$2,d3
	lsl.w	#$2,d4
	subq.w	#$2,d4
	add.w	d4,d2
loc_010144:
	move.w	d0,(a0)+
loc_010146:
	move.w	d1,-(a1)
	addq.w	#$1,d0
	addq.w	#$4,d3
	sub.w	d3,d2
	bpl.s	loc_010146
	add.w	d4,d2
	subq.w	#$4,d4
	subq.w	#$1,d1
	cmp.w	d0,d1
	bge.s	loc_010144
	rts
