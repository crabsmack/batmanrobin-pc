; ---------------------------------------------------------------------------
; ToyChain_MakeChild8B - inline chain maker of ToyChain_Spawn8B: an 8-figure chain on ToyChain_MakeChild7B's link table $039AC4 at fixed speed $100, sway cruise $255, brake distance $50
; In:  a6 = the base (ToyChain_Spawn8B, field $2A); inline dc.w 8, dc.l $039AC4, dc.l $0399B4
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0008 00039AC4 000399B4`; Op2_SpawnChild 002A dc.l at $0398A6 (Scr2_0398A2); no corpus run at 4:3 (0399A6.syn)
; ---------------------------------------------------------------------------
ToyChain_MakeChild8B:
	jsr	ToyChain_SpawnInline(pc)
