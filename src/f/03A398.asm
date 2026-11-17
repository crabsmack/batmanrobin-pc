; ---------------------------------------------------------------------------
; ToyChain_MakeBox4 - inline chain maker of a 4-figure chain for the roaming gift boxes (table $03A54A: growth $266, energy $20), sway oscillator limit $C00 at speed $255
; In:  a6 = the gift box (field $2A); inline dc.w 4, dc.l $03A54A, dc.l $03A3A6
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0004 0003A54A 0003A3A6`; Op2_SpawnChild 002A in Enemy_GiftBox_SpawnRoam4A/4B ($03A048, $03A23C); TAS f83007 (section $3100). The random-pick makers after it ($03A3BE / $03A3E4: 4 figures, table $03A566, sway speed $D5 / $12A) are never run, still raw
; ---------------------------------------------------------------------------
ToyChain_MakeBox4:
	jsr	ToyChain_SpawnInline(pc)
