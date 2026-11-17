; ---------------------------------------------------------------------------
; TextMap_CmdNumberPlus1 - text-map command $96: writes the word at a long pointer plus one as a decimal number without leading zeros (1-based setting numbers)
; In:  a0 = operand: long pointer to a word
; Out: a6 advanced; next command
; Evidence: addq.l #1 then the TextMap_CmdNumber digit chain; the options screen shows the music/sound test numbers from $F60A/$F60C, which $499E8 plays as value+1; scenario-options
; ---------------------------------------------------------------------------
TextMap_CmdNumberPlus1:
	moveq	#$0,d0
	movea.l	(a0)+,a1
	move.w	(a1)+,d0
	addq.l	#$1,d0
	bra.s	loc_014450
; ---------------------------------------------------------------------------
; TextMap_CmdNumber - text-map command $A0: writes the word at a long pointer as a decimal number of up to 5 digits without leading zeros
; In:  a0 = operand: long pointer to a word
; Out: a6 advanced; next command
; Evidence: divu #10000/#1000/#100/#10 through TextMap_PutDigit, last digit always written; computed entry in scenario-continue
; ---------------------------------------------------------------------------
TextMap_CmdNumber:
	moveq	#$0,d0
	movea.l	(a0)+,a1
	move.w	(a1)+,d0
loc_014450:
	moveq	#$0,d1
	divu.w	#$2710,d0
	bsr.w	TextMap_PutDigit
	divu.w	#$3E8,d0
	bsr.w	TextMap_PutDigit
	divu.w	#$64,d0
	bsr.w	TextMap_PutDigit
	divu.w	#$A,d0
	bsr.w	TextMap_PutDigit
	add.w	d0,d0
	move.w	$20(a5,d0.w),d0
	add.w	d6,d0
	move.w	d0,(a6)+
	bra.w	TextMap_NextCmd
