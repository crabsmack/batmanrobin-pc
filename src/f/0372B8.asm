; ---------------------------------------------------------------------------
; ToyChain_RemoveAllLinks - removes every link of a chain (ToyChain_RemoveLastLink until it reports none left)
; In:  a6 = chain object, a4 = its a4 block
; Out: link count 0; d0 = 1
; Evidence: loop bsr ToyChain_RemoveLastLink / beq; called by ToyChain_KillChild and by the chain death path $037A64 (Op2_CallNativeRel then Op2_Kill)
; ---------------------------------------------------------------------------
ToyChain_RemoveAllLinks:
	bsr.w	ToyChain_RemoveLastLink
	beq.s	ToyChain_RemoveAllLinks
	rts
