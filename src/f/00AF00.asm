; ---------------------------------------------------------------------------
; VInt_BusyMode3C - busy-path V-int scroll slot for mode $3C: bra.w HInt_Rearm, as mode $38
; In:  g_ScrollMode = $3C
; Out: as HInt_Rearm
; Evidence: VInt_BusyScrollTbl offset $3C, after VInt_BusyMode38 (the same branch); the stage-4 scenes set mode $3C ($03BB88); never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
VInt_BusyMode3C:
	bra.w	HInt_Rearm
