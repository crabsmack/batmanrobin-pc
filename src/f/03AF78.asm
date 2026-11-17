; ---------------------------------------------------------------------------
; ToyChain_MakeHanging8B - chain-child spawner of Enemy_PropToy_SpawnTwinChain8 (both chains): ToyChain_SpawnInline with 8 links, table $03B190 and script $03AF86 (random speed $33-$E6, slow sway oscillator $500 at $80)
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A and $2C dc.l at $03AB8C / $03AB98 in Enemy_PropToy_SpawnTwinChain8's script; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeHanging8B:
	jsr	ToyChain_SpawnInline(pc)
