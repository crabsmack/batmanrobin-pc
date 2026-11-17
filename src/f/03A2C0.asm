; ---------------------------------------------------------------------------
; ToyChain_MakeBox10A - inline chain maker for Enemy_GiftBox_SpawnStay10A: 10 figures that grow out of the opened box (table $03A4A2: growth $266, energy $50, first word -$18) on the swaying body $037DD6 (oscillator X $40/$55/$55/$78)
; In:  a6 = the gift box (field $2A); inline dc.w $A, dc.l $03A4A2, dc.l $03A2CE
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `000A 0003A4A2 0003A2CE`; script `5046 0008` (Op2_AllocOsc) + Op2_SetOscX, goto $037DD6 (Op2_StepOsc + ToyChain_Update); Op2_SpawnChild 002A at $039D3E; TAS f80229-f91205 (3 runs)
; ---------------------------------------------------------------------------
ToyChain_MakeBox10A:
	jsr	ToyChain_SpawnInline(pc)
