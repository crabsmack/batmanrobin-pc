; ---------------------------------------------------------------------------
; VInt_BusyScrollTbl28 - busy-path scroll jump table, modes $28-$2C: $28 re-arms the H-int raster (VInt_BusyRearmRaster), $2C only turns the display back on
; In:  as VInt_BusyScroll
; Out: reg 1 = $64; mode $28 also re-arms the H-int on the same line table
; Evidence: bra.w $AFDA at $AEEC, bra.w $AF04 at $AEF0; computed targets of the jmp at $AEC0 in 5 / 11 corpus runs; $AEF4 (mode $30) is never read
; ---------------------------------------------------------------------------
VInt_BusyScrollTbl28:
	bra.w	VInt_BusyRearmRaster
; ---------------------------------------------------------------------------
; VInt_BusyMode2C - busy-path V-int scroll slot for mode $2C: bra.w VInt_ScrollDisplayOn (no raster to re-arm in this mode)
; In:  as VInt_BusyScroll
; Out: VDP reg 1 = $64
; Evidence: bra.w $AF04 at $AEF0, entered by the jmp at $AEC0 when g_ScrollMode = $2C (11 corpus runs)
; ---------------------------------------------------------------------------
VInt_BusyMode2C:
	bra.w	VInt_ScrollDisplayOn
