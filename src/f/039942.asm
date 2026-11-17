; ---------------------------------------------------------------------------
; ToyChain_MakeChild5B - inline chain maker of ToyChain_Spawn5B: a 5-figure chain (energy $90) that sways fast: speed $33-$100, sway cruise $2AA, short brake distance $50
; In:  a6 = the base (ToyChain_Spawn5B, field $2A); inline dc.w 5, dc.l Tbl_039A0A+$BA, dc.l $039950
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0005 00039AC4 00039950`; Op2_SpawnChild 002A in Scr2_039836 (spawn entries Tbl_034142+$4C4); TAS f110618 (section $3400 card hall)
; ---------------------------------------------------------------------------
ToyChain_MakeChild5B:
	jsr	ToyChain_SpawnInline(pc)
