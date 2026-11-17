; ---------------------------------------------------------------------------
; ToyChain_MakeChild5 - chain-child spawner for Op2_SpawnChild: ToyChain_SpawnInline with 5 links, parameter table $039AC4 (shootable links, heading $300, $90 energy per link) and script $0398C8 (sway speed $280, then the chain body $0379DA)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $039AC4 / dc.l $0398C8; dc.l $0398BA in Scr2_0397A6 (ToyChain_Spawn5); TAS f83916-f86444 (3 frames)
; ---------------------------------------------------------------------------
ToyChain_MakeChild5:
	jsr	ToyChain_SpawnInline(pc)
