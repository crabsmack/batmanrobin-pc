; ---------------------------------------------------------------------------
; ToyChain_MakeBox4C - inline chain maker of a 4-figure chain (table $03A566) for Enemy_GiftBox_SpawnRoamRandom: sway oscillator limit $C00 at speed $12A
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $03A138: the second random pick of Enemy_GiftBox_SpawnRoamRandom's subroutine $03A11C; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeBox4C:
	jsr	ToyChain_SpawnInline(pc)
