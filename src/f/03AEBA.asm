; ---------------------------------------------------------------------------
; ToyChain_MakeHanging5E - chain-child spawner of Enemy_PropToy_SpawnChain5RightC: ToyChain_SpawnInline with 5 links, table $03B158 and script $03AEC8 (random speed $33-$E6, sway oscillator $800 at $12A)
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $03ABF2 in Enemy_PropToy_SpawnChain5RightC's script; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeHanging5E:
	jsr	ToyChain_SpawnInline(pc)
