; ---------------------------------------------------------------------------
; ToyChain_MakeChild7B - inline chain maker of ToyChain_Spawn7B: a 7-figure chain (energy $90) at fixed speed $100, sway cruise $280, brake distance $50
; In:  a6 = the base (ToyChain_Spawn7B, field $2A); inline dc.w 7, dc.l Tbl_039A0A+$BA, dc.l $039994
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0007 00039AC4 00039994`; Op2_SpawnChild 002A in Scr2_03987E (spawn entries Tbl_034142+$4E4); not in the TAS: fuzz-109800-s1 f113365 (section $3400). The makers after it, $0399A6 (8 links) and $0399C6/$0399E6 (10 links, energy $20/$200), are never run, still raw
; ---------------------------------------------------------------------------
ToyChain_MakeChild7B:
	jsr	ToyChain_SpawnInline(pc)
