; ---------------------------------------------------------------------------
; Child_GetA4 - helper for the child-object ops: reads a word a6 field offset and returns the child's a4 block; with no child it drops its caller's return address and continues the script (the op does nothing)
; In:  a2 = script: word a6 field offset; a6 = parent
; Out: a0 = child's a4 block (via Child_A4OfHandle), d1 = child a6; or NEXT straight from here when the field is 0
; Evidence: addq.l #4,a7 then NEXT on 0; bsr'd by Op2_StopChildCoroutine1 ($2AEA) and the never-run child ops $2AC2/$2AD0/$2ADE; 10 runs
; ---------------------------------------------------------------------------
Child_GetA4:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d1
	bne.s	Child_A4OfHandle
	addq.l	#$4,a7
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Child_GetA4Skip2 - as Child_GetA4 for child ops with one more word operand: with no child it also skips that word before continuing the script
; In:  a2 = script: word a6 field offset, word operand of the calling op
; Out: a0 = child's a4 block; or a2 += 2 and NEXT when the field is 0
; Evidence: addq.l #2,a2 / addq.l #4,a7 / NEXT; bsr'd by Op2_StartChildCoroutine0Rel..3Rel; $269A (never run) is the 4-byte-operand twin; 32 runs
; ---------------------------------------------------------------------------
Child_GetA4Skip2:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d1
	bne.s	Child_A4OfHandle
	addq.l	#$2,a2
	addq.l	#$4,a7
	movea.w	(a2)+,a0
	jmp	(a0)
