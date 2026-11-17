; ---------------------------------------------------------------------------
; TextMap_ResumeTyping - typewriter step of TextMap_Run: adds the speed to the accumulator and, on carry, writes the next character of the saved text run
; In:  g_TextMapTypeSpeed/Acc, the saved list pointer, count and write pointer
; Out: a1-a6 restored; returns at once when no character is due
; Evidence: add.w $F3BC,$F3BA / bcs; restores a6 $F3B6, a0 $F3C0, d7 $F3BE saved by TextMap_PutChars; taken when $F4EE is set (ending text, scenario-ending)
; ---------------------------------------------------------------------------
TextMap_ResumeTyping:
	move.w	(g_TextMapTypeSpeed).w,d0
	add.w	d0,(g_TextMapTypeAcc).w
	bcs.s	loc_0143F4
	movem.l	(a7)+,a1-a6
	rts
loc_0143F4:
	movea.l	(g_TextMapTypeDst).w,a6
	movea.l	(g_TextMapTypeSrc).w,a0
	move.w	(g_TextMapTypeCount).w,d7
	bra.s	TextMap_PutCharsNext
