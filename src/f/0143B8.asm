; ---------------------------------------------------------------------------
; TextMap_CmdGoto - text-map command $E: continues the list at the word offset that follows (relative to the offset word)
; In:  a0 = operand: word offset
; Out: next command
; Evidence: adda.w (a0),a0; also the tail of TextMap_CmdMenuItem / TextMap_CmdBlinkGoto; scenario-options
; ---------------------------------------------------------------------------
TextMap_CmdGoto:
	adda.w	(a0),a0
	bra.s	TextMap_NextCmd
; ---------------------------------------------------------------------------
; TextMap_CmdReturn - text-map command $12: returns from a sub-list entered by TextMap_CmdMenuItem (pops the list pointer it pushed)
; In:  a0 pushed on the stack
; Out: next command after the menu item
; Evidence: movea.l (a7)+,a0 pairs with the pea 6(a0) of TextMap_CmdMenuItem; computed entry in scenario-level-skip and 4 other runs
; ---------------------------------------------------------------------------
TextMap_CmdReturn:
	movea.l	(a7)+,a0
	bra.s	TextMap_NextCmd
