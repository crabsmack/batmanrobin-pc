; ---------------------------------------------------------------------------
; ToyChain_MakeHanging5D - chain-child spawner: ToyChain_SpawnInline with 5 links, table $03B1AC (link energy 1) and script $03B042 (charge cycle $038810, coroutine 0 $038262, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $03B1AC / dc.l $03B042; dc.l $03B034 in Scr2_03ACCA+8; TAS f93782-f95691
; ---------------------------------------------------------------------------
ToyChain_MakeHanging5D:
	jsr	ToyChain_SpawnInline(pc)
