; ---------------------------------------------------------------------------
; ToyChain_MakeBox6 - inline chain maker of a 6-figure chain (table $03A566) for Enemy_GiftBox_SpawnRoamRandom: sway oscillator limit $C00 at speed $1D5
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $03A14C: the fourth random pick of Enemy_GiftBox_SpawnRoamRandom's subroutine $03A11C (the third is ToyChain_MakeBox5); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeBox6:
	jsr	ToyChain_SpawnInline(pc)
