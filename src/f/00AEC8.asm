; ---------------------------------------------------------------------------
; VInt_BusyMode04 - busy-path V-int scroll slot for mode $04: bra.w VInt_ScrollDisplayOn, so a dropped frame uploads no scroll
; In:  g_ScrollMode = this slot's offset
; Out: as VInt_ScrollDisplayOn
; Evidence: VInt_BusyScrollTbl ($AEC4, jmp (pc,d0.w) on g_ScrollMode) offset $04, a mode the stage-4 scenes set ($03BB88); never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
VInt_BusyMode04:
	bra.w	VInt_ScrollDisplayOn
