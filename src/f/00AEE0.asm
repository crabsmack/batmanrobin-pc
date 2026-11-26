; ---------------------------------------------------------------------------
; VInt_BusyMode1C - busy-path V-int scroll slot for mode $1C: bra.w VInt_ScrollDisplayOn, so a dropped frame uploads no scroll
; In:  g_ScrollMode = this slot's offset
; Out: as VInt_ScrollDisplayOn
; Evidence: VInt_BusyScrollTbl ($AEC4, jmp (pc,d0.w) on g_ScrollMode) offset $1C, a mode nothing sets (every g_ScrollMode store is an immediate: 4 8 $C $10 $28 $2C $34 $38 $3C); never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
VInt_BusyMode1C:
	bra.w	VInt_ScrollDisplayOn
