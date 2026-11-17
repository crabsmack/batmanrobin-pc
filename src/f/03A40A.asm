; ---------------------------------------------------------------------------
; ToyChain_MakeBox5 - inline chain maker of a 5-figure chain (table $03A566: growth $200, energy $10), sway oscillator $C00 at speed $180: one of the four random picks of Enemy_GiftBox_SpawnRoamRandom
; In:  a6 = the gift box (field $2A); inline dc.w 5, dc.l $03A566, dc.l $03A418
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0005 0003A566 0003A418`; Op2_SpawnChild 002A at $03A142 in the random subroutine $03A11C (Op2_SwitchRandom); TAS f83581, the pick the TAS drew. The 6-figure pick after it ($03A430, speed $1D5) is never run, still raw
; ---------------------------------------------------------------------------
ToyChain_MakeBox5:
	jsr	ToyChain_SpawnInline(pc)
