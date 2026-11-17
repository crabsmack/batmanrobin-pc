; ---------------------------------------------------------------------------
; ToyChain_MakeDropIn12 - inline chain maker of ToyChain_SpawnDropIn12: 12 figures (table $039C0C, energy $B0) on the chain body $037B8A (sway cruise $155)
; In:  a6 = the drop-in base (field $2A); inline dc.w $C, dc.l $039C0C, dc.l $037B8A
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `000C 00039C0C 00037B8A`; Op2_SpawnChild 002A in Scr2_039B84; TAS f92263-f92886 (6 runs)
; ---------------------------------------------------------------------------
ToyChain_MakeDropIn12:
	jsr	ToyChain_SpawnInline(pc)
