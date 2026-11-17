; ---------------------------------------------------------------------------
; ToyChain_MakeChild10 - inline chain maker of ToyChain_Spawn10A/B: a tall 10-figure chain (table Tbl_039A0A+$F2, energy $90), speed $100, slow sway cruise $D5
; In:  a6 = the base (ToyChain_Spawn10A/10B and three raw bases after them, field $2A); inline dc.w $A, dc.l $039AFC, dc.l $039A14
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `000A 00039AFC 00039A14`; Op2_SpawnChild 002A at $0396AE/$0396D2/$0396F6/$03971A/$03973E; TAS f86799 (section $3100). The makers inside Tbl_039A0A after it ($039A26 14 links, $039A46 11 links energy $1000, $039A66, $039A88) are never run, still raw
; ---------------------------------------------------------------------------
ToyChain_MakeChild10:
	jsr	ToyChain_SpawnInline(pc)
