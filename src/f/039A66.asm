; ---------------------------------------------------------------------------
; ToyChain_MakeChild7C - inline chain maker of ToyChain_Spawn7C: a 7-figure chain (table $039AE0) at a random speed $4C-$100, sway cruise $155, brake distance $78
; In:  a6 = the base (ToyChain_Spawn7C, field $2A); inline dc.w 7, dc.l $039AE0, dc.l $039A74
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0007 00039AE0 00039A74`; Op2_SpawnChild 002A dc.l at $039786 (Scr2_039782); no corpus run at 4:3 (039A66.syn)
; ---------------------------------------------------------------------------
ToyChain_MakeChild7C:
	jsr	ToyChain_SpawnInline(pc)
