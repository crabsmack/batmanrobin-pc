; ---------------------------------------------------------------------------
; Title_ReadPadAndCursor - title director per-frame native: copies the menu cursor into script variable 1 and pad 1's newly pressed buttons into variable 0 for the menu branches
; In:  a4 = the director's a4 block
; Out: $24(a4) = g_MenuCursor, $22(a4) = g_Pad1+2; jsr $7FE.w (step coroutines, yield)
; Evidence: native at $496EA in Scr2_0496D8's frame loop; the coroutines test var 0 bit 7 (Start) and var 1 = 1 (OPTIONS); every title frame (idle 4680 frames, scenario-options f404-f1770)
; ---------------------------------------------------------------------------
Title_ReadPadAndCursor:
	move.w	(g_MenuCursor).w,$24(a4)
	moveq	#$0,d0
	move.b	(g_Pad1+2).w,d0
	move.w	d0,$22(a4)
	jsr	($7FE).w
