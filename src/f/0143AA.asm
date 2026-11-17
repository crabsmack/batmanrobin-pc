; ---------------------------------------------------------------------------
; TextMap_CmdEnd - text-map command 0: ends the list, restores a1-a6 and clears the typewriter flag
; In:  a1-a6 pushed by TextMap_Run
; Out: returns to TextMap_Run's caller
; Evidence: offset $0000 ends every list in Tbl_0498EA; movem.l (a7)+,a1-a6 / sf.b $F4EE; 9 runs
; ---------------------------------------------------------------------------
TextMap_CmdEnd:
	movem.l	(a7)+,a1-a6
	sf.b	(g_TextMapTyping).w
	rts
