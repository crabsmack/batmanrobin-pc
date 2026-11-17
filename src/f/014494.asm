; ---------------------------------------------------------------------------
; TextMap_CmdMenuItem - text-map command $EA: calls one of two sub-lists for a menu item: the alternate one when the item is the cursor's and the 8-frame blink phase is on, else the normal one
; In:  a0 = operands: word item index, word offset normal, word offset highlighted
; Out: sub-list runs, TextMap_CmdReturn comes back after the operands
; Evidence: pea 6(a0); cmp.w $F3B4 (menu cursor); btst #3 of the frame counter's low byte $E031; options items '00EA 0000 001E 0036'; scenario-options and 4 runs
; ---------------------------------------------------------------------------
TextMap_CmdMenuItem:
	pea.l	$6(a0)
	move.w	(a0)+,d0
	cmp.w	(g_MenuCursor).w,d0
	bne.w	TextMap_CmdGoto
	btst	#$3,(g_FrameCounter+3).w
	beq.w	TextMap_CmdGoto
	addq.l	#$2,a0
	bra.w	TextMap_CmdGoto
