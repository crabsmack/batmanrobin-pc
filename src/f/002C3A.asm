; ---------------------------------------------------------------------------
; Op2_SetCounter0 - a2 op: sets loop counter 0 ($14(a4)), counted per run by Op2_LoopCounter0 (not by the tick like the timers)
; In:  a2 = script: word count
; Out: NEXT
; Evidence: move.w (a2)+,$14(a4); 109 runs; e.g. $1377E `2C3A 0005 ... 2C54 FFF0`
; ---------------------------------------------------------------------------
Op2_SetCounter0:
	move.w	(a2)+,$14(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetCounter0Rand - a2 op: sets loop counter 0 ($14(a4)) to base + Rand_Below(range)
; In:  a2 = script: word base, word range
; Out: NEXT
; Evidence: d1 = base, d0 = range, bsr Rand_Below, add; 9 runs; $38B52 `2C42 0003 0002` = 3..4; $2C6A/$2C92 (never run) are the counter 1/2 twins
; ---------------------------------------------------------------------------
Op2_SetCounter0Rand:
	move.w	(a2)+,d1
	move.w	(a2)+,d0
	bsr.w	Rand_Below
	add.w	d1,d0
	move.w	d0,$14(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_LoopCounter0 - a2 op: decrements loop counter 0 ($14(a4)) and branches while it is still above 0 (a counted loop back)
; In:  a2 = script: word offset
; Out: branch via Op2_Goto while > 0, else NEXT after the offset
; Evidence: subq.w #1,$14(a4) / bgt Op2_Goto; 116 runs
; ---------------------------------------------------------------------------
Op2_LoopCounter0:
	subq.w	#$1,$14(a4)
	bgt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetCounter1 - a2 op: sets loop counter 1 ($16(a4)) for Op2_LoopCounter1
; In:  a2 = script: word count
; Out: NEXT
; Evidence: move.w (a2)+,$16(a4); 38 runs; $255DC `2C62 0003`
; ---------------------------------------------------------------------------
Op2_SetCounter1:
	move.w	(a2)+,$16(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
