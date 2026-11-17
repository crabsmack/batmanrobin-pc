; ---------------------------------------------------------------------------
; ToyChain_MakeHanging5B - chain-child spawner: ToyChain_SpawnInline with 5 links, table $03B1AC (link energy 1) and script $03AFE6 (charge cycle $038810 only, no coroutine 0, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $03B1AC / dc.l $03AFE6; dc.l $03AFD8 in Scr2_03AC8E+8; TAS f94297-f95872
; ---------------------------------------------------------------------------
ToyChain_MakeHanging5B:
	jsr	ToyChain_SpawnInline(pc)
