; ---------------------------------------------------------------------------
; ToyChain_MakeBox8 - inline chain maker of an 8-figure chain for the stay-put gift boxes (table $03A582: growth $200, energy $10), sway oscillator $C00 at speed $1D5
; In:  a6 = the gift box (field $2A); inline dc.w 8, dc.l $03A582, dc.l $03A464
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0008 0003A582 0003A464`; Op2_SpawnChild 002A in Enemy_GiftBox_SpawnStay8A/8B ($03A188, $03A1E2); TAS f83761-f83792 (section $3100). The 7-figure maker after it ($03A47C, table $03A59E) is never run, still raw
; ---------------------------------------------------------------------------
ToyChain_MakeBox8:
	jsr	ToyChain_SpawnInline(pc)
