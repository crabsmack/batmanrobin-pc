; ---------------------------------------------------------------------------
; ToyChain_MakeHanging5C - chain-child spawner: ToyChain_SpawnInline with 5 links, table $03B1AC (link energy 1) and script $03B012 (same words as $03AFB6: coroutine 3 $03829C, coroutine 0 $038262, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 5 / dc.l $03B1AC / dc.l $03B012; dc.l $03B004 in Scr2_03ACAE+8; TAS f93540-f95903
; ---------------------------------------------------------------------------
ToyChain_MakeHanging5C:
	jsr	ToyChain_SpawnInline(pc)
