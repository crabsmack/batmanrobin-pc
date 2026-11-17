; ---------------------------------------------------------------------------
; HUD_StepIdle - the HUD step jump table's offset-0 entry: no DMA this frame (the following nop pads the table to the HUD_StepBranches entries)
; In:  none
; Out: none
; Evidence: jmp $11310(pc,d0.w) at $1130C with d0 = g_HUDStep in 0,4,8,$C,$10,$14; step 0 only when nothing set g_HUDStep
; ---------------------------------------------------------------------------
HUD_StepIdle:
	rts
loc_011312:
	nop
