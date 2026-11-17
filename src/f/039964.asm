; ---------------------------------------------------------------------------
; ToyChain_MakeChild6B - inline chain maker of ToyChain_Spawn6B: a 6-figure fast-swaying chain (energy $90), speed $33-$100, sway cruise $2AA, brake distance $50
; In:  a6 = the base (ToyChain_Spawn6B, field $2A); inline dc.w 6, dc.l Tbl_039A0A+$BA, dc.l $039972
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0006 00039AC4 00039972`; Op2_SpawnChild 002A in Scr2_03985A (spawn entries Tbl_034142+$4D4); not in the TAS: fuzz-109800-s1/s2 f111750-f113080 (section $3400)
; ---------------------------------------------------------------------------
ToyChain_MakeChild6B:
	jsr	ToyChain_SpawnInline(pc)
