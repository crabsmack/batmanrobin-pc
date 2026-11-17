; ---------------------------------------------------------------------------
; TextMap_CmdText - text-map command $58: writes a run of characters (count byte, then ASCII) as font tiles at the write position, then realigns the list to a word
; In:  a0 = operand: byte count, count characters
; Out: a6 advanced; next command
; Evidence: moveq #0,d7 / move.b (a0)+,d7 into the TextMap_PutChars loop; '0058 07 OPTIONS' in Tbl_0498EA; 9 runs
; ---------------------------------------------------------------------------
TextMap_CmdText:
	moveq	#$0,d7
	move.b	(a0)+,d7
	bra.s	TextMap_PutCharsNext
