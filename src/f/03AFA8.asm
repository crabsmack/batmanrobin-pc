; ---------------------------------------------------------------------------
; ToyChain_MakeHanging5A - chain-child spawner: ToyChain_SpawnInline with 5 links, table $03B1AC (link energy 1: one hit breaks a link) and script $03AFB6 (coroutine 3 $03829C, coroutine 0 $038262, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $03B1AC / dc.l $03AFB6; table word +$E = 1 (copied to the chain block +$20 and $52(a6)); dc.l $03AFA8 in Scr2_03AC6E+8; TAS f94419-f95660
; ---------------------------------------------------------------------------
ToyChain_MakeHanging5A:
	jsr	ToyChain_SpawnInline(pc)
