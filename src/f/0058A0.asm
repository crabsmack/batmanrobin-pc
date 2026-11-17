; ---------------------------------------------------------------------------
; Op4_AddAccelY - a4 op: adds a signed 8.8 word to the object's y acceleration $52
; In:  a4 = script: word day (8.8)
; Out: NEXT
; Evidence: ext.l / asl.l #8 / add.l to $52(a6); script use $3B732; 3 corpus runs
; ---------------------------------------------------------------------------
Op4_AddAccelY:
	move.w	(a4)+,d0
	ext.l	d0
	asl.l	#$8,d0
	add.l	d0,$52(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
