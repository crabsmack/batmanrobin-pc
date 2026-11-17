; ---------------------------------------------------------------------------
; VInt_BusyRearmRaster - busy-path entry for mode $28: re-arms the H-int raster on the table already shown (no swap)
; In:  a0 = VDP control port
; Out: as HInt_Rearm
; Evidence: bra HInt_Rearm; target of VInt_BusyScrollTbl28 entry $28; 5 corpus runs (fuzz 113400/115200, level-skip)
; ---------------------------------------------------------------------------
VInt_BusyRearmRaster:
	bra.w	HInt_Rearm
