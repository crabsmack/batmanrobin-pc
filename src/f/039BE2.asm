; ---------------------------------------------------------------------------
; ToyChain_MakeDropIn9 - inline chain maker of ToyChain_SpawnDropIn9: 9 figures (table $039C28, energy $90) on the chain body $037B8A
; In:  a6 = the drop-in base (field $2A); inline dc.w 9, dc.l $039C28, dc.l $037B8A
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0009 00039C28 00037B8A`; Op2_SpawnChild 002A in Scr2_039BAC; TAS f93331-f93388
; ---------------------------------------------------------------------------
ToyChain_MakeDropIn9:
	jsr	ToyChain_SpawnInline(pc)
