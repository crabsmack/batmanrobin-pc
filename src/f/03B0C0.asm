; ---------------------------------------------------------------------------
; ToyChain_MakeHanging10C - first chain of the twin-chain propeller toy: ToyChain_SpawnInline with 10 links, table $03B1E4 (link energy $20) and script $03B0CE (sway oscillator $AA, charge cycle $038828, coroutine 0 $03823A, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w $A / dc.l $03B1E4 / dc.l $03B0CE; dc.l $03B0C0 in Scr2_03AE52+8 (child field $2A); TAS f97692
; ---------------------------------------------------------------------------
ToyChain_MakeHanging10C:
	jsr	ToyChain_SpawnInline(pc)
