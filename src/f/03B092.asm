; ---------------------------------------------------------------------------
; ToyChain_MakeHanging6D - chain-child spawner of Enemy_PropToy_SpawnTwinChain6's second chain: ToyChain_SpawnInline with 6 links, table $03B1C8 and script $03B0A0 (speed $E6, sway oscillator $800 at $12A)
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2C dc.l at $03ADD6 in Enemy_PropToy_SpawnTwinChain6's script; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeHanging6D:
	jsr	ToyChain_SpawnInline(pc)
