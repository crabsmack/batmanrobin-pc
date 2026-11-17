; ---------------------------------------------------------------------------
; VInt_BusyScrollTbl - jump table of the busy-path scroll modes: entry 0 branches to the display-on routine $AF04 (entries 8-$10 follow in the next file)
; In:  as VInt_BusyScroll
; Out: reg 1 = $64
; Evidence: bra.w $AF04; entry 4 ($AEC8) never read; 18 corpus runs
; ---------------------------------------------------------------------------
VInt_BusyScrollTbl:
	bra.w	VInt_ScrollDisplayOn
