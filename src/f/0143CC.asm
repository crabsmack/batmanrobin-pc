; ---------------------------------------------------------------------------
; TextMap_CmdSetPos - text-map command $22: moves the write position to a byte pair (column*2, row*2) of the 40-column plane image
; In:  a0 = operand bytes x, y (signed)
; Out: a6 = $FF9000 + x + y*$28
; Evidence: mulu.w #$28 on y; Tbl_0498EA '0022 2006' then 'OPTIONS' (7 letters) centred at column 16, row 3; 9 runs
; ---------------------------------------------------------------------------
TextMap_CmdSetPos:
	move.b	(a0)+,d0
	ext.w	d0
	move.b	(a0)+,d1
	ext.w	d1
	mulu.w	#$28,d1
	add.w	d1,d0
	lea.l	(g_SceneWorkBuf).l,a6
	adda.w	d0,a6
	bra.s	TextMap_NextCmd
