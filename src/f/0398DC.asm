; ---------------------------------------------------------------------------
; ToyChain_MakeChild6 - chain-child spawner for Op2_SpawnChild: ToyChain_SpawnInline with 6 links, table $039AC4, script $0398EA (sway speed $255, chain body $0379DA)
; In:  d4/d5 = position
; Out: d0 = 0 / a0 = chain block, or nonzero
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 6 / dc.l $039AC4 / dc.l $0398EA; dc.l $0398DC in Scr2_0397CA (ToyChain_Spawn6); TAS f84217-f85120 (4 frames)
; ---------------------------------------------------------------------------
ToyChain_MakeChild6:
	jsr	ToyChain_SpawnInline(pc)
