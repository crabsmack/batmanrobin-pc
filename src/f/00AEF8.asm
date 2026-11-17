; ---------------------------------------------------------------------------
; VInt_BusyScrollTbl34 - busy-path scroll jump table, modes $34-$3C: re-arm the H-int raster (HInt_Rearm) so the per-line effect survives a dropped frame
; In:  as VInt_BusyScroll
; Out: H-int re-armed on g_HIntTblFront; reg 1 = $64
; Evidence: bra.w $B1D8 at $AEF8/$AEFC (and $AF00, mode $3C, never read); computed targets of the jmp at $AEC0 in 15 / 11 corpus runs; TAS frames 39894-46233 (stage 2)
; ---------------------------------------------------------------------------
VInt_BusyScrollTbl34:
	bra.w	HInt_Rearm
; ---------------------------------------------------------------------------
; VInt_BusyMode38 - busy-path V-int scroll slot for mode $38: bra.w HInt_Rearm, keeping the per-line raster alive over a dropped frame
; In:  as VInt_BusyScroll
; Out: H-int re-armed on g_HIntTblFront; reg 1 = $64
; Evidence: bra.w $B1D8 at $AEFC, entered by the jmp at $AEC0 when g_ScrollMode = $38 (11 corpus runs)
; ---------------------------------------------------------------------------
VInt_BusyMode38:
	bra.w	HInt_Rearm
