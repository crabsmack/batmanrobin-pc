; ---------------------------------------------------------------------------
; ToyChain_MakeHanging6C - chain-child spawner of Enemy_PropToy_SpawnTwinChain6's first chain: ToyChain_SpawnInline with 6 links, table $03B1C8 and script $03B072 (speed $100, sway oscillator $800 at $12A)
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $03ADCA in Enemy_PropToy_SpawnTwinChain6's script; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeHanging6C:
	jsr	ToyChain_SpawnInline(pc)
