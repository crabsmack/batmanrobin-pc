; ---------------------------------------------------------------------------
; ToyChain_MakeBox10C - inline chain maker for Enemy_GiftBox_SpawnStay10C: 10 figures (table $03A4F6) that grow out of the opened box on the swaying body $037DD6 (oscillator X limit $1E00, speed $100)
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $039C72 in Enemy_GiftBox_SpawnStay10C's coroutine 0; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeBox10C:
	jsr	ToyChain_SpawnInline(pc)
