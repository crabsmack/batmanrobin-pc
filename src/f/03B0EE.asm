; ---------------------------------------------------------------------------
; ToyChain_MakeHanging10D - second chain of the twin-chain propeller toy: ToyChain_SpawnInline with 10 links, table $03B200 (link energy $20) and script $03B0FC (sway oscillator $D5, charge cycle $038840, coroutine 0 $03824E, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w $A / dc.l $03B200 / dc.l $03B0FC; dc.l $03B0EE in Scr2_03AE52+$14 (child field $2C); TAS f97692
; ---------------------------------------------------------------------------
ToyChain_MakeHanging10D:
	jsr	ToyChain_SpawnInline(pc)
