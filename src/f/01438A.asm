; ---------------------------------------------------------------------------
; TextMap_RunInline - a2 native: runs the text-map command list whose address is the script's next dc.l (TextMap_Run)
; In:  a2 = script at the dc.l
; Out: a2 past the dc.l, then as TextMap_Run
; Evidence: movea.l (a2)+,a0 falls into TextMap_Run; dc.l $0001438A in the ending scripts ($4A60A, $4A750, $4A784 ...); scenario-ending
; ---------------------------------------------------------------------------
TextMap_RunInline:
	movea.l	(a2)+,a0
