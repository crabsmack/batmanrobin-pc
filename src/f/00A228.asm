; ---------------------------------------------------------------------------
; Scene_ResetBufPtrs - points all 16 entries of the scene-script buffer pointer table $E092 at RAM $FF0000
; In:  none
; Out: $E092-$E0D1 = $FF0000 x 16
; Evidence: caller Scene_InitCommon; the a6 ops $70E6/$7108/$7120/$7148/$7158 index the table with a word operand (decompress destinations, $7148 stores $E08E into an entry); 25 TAS frames
; ---------------------------------------------------------------------------
Scene_ResetBufPtrs:
	lea.l	(g_SceneBuf&$FFFFFF).l,a0
	lea.l	(g_BufPtrs).l,a1
	moveq	#$F,d7
loc_00A236:
	move.l	a0,(a1)+
	dbra	d7,loc_00A236
	rts
