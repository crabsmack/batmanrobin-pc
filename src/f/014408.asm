; ---------------------------------------------------------------------------
; TextMap_PutChars - character loop of TextMap_CmdText: maps each ASCII byte through the font map (from $20) plus the tile base into the plane image; in typewriter mode it saves its state after one character and leaves
; In:  a0 = characters, d7 = count, a5 = font map, d6 = tile base, a6 = write pointer
; Out: a6 advanced; typewriter: a0/d7/a6 saved to $F3C0/$F3BE/$F3B6 and returns
; Evidence: move.w -$40(a5,d0.w),d0 (char*2 - $40 = entries from space) / add.w d6,d0 / move.w d0,(a6)+; tst.b $F4EE; font map Tbl_0586F2
; ---------------------------------------------------------------------------
TextMap_PutChars:
	moveq	#$0,d0
	move.b	(a0)+,d0
	add.w	d0,d0
	move.w	-$40(a5,d0.w),d0
	add.w	d6,d0
	move.w	d0,(a6)+
	tst.b	(g_TextMapTyping).w
	beq.s	TextMap_PutCharsNext
	move.l	a0,(g_TextMapTypeSrc).w
	move.w	d7,(g_TextMapTypeCount).w
	move.l	a6,(g_TextMapTypeDst).w
	movem.l	(a7)+,a1-a6
	rts
; ---------------------------------------------------------------------------
; TextMap_PutCharsNext - the step of the TextMap_PutChars loop: the next character while the dbra count lasts, else the list realigned to a word and the next command
; In:  d7 = characters left, a0 = the next character
; Out: a0 word-aligned at the run's end; next command
; Evidence: dbra d7,TextMap_PutChars / move.w a0,d0 / lsr.w #1 / bcc TextMap_NextCmd / addq.l #1,a0; entered by TextMap_CmdText (the first character), TextMap_PutChars' loop and TextMap_ResumeTyping (the typewriter's next character)
; ---------------------------------------------------------------------------
TextMap_PutCharsNext:
	dbra	d7,TextMap_PutChars
	move.w	a0,d0
	lsr.w	#$1,d0
	bcc.w	TextMap_NextCmd
	addq.l	#$1,a0
	bra.w	TextMap_NextCmd
