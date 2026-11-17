; ---------------------------------------------------------------------------
; ToyChain_MakeBox10B - inline chain maker for Enemy_GiftBox_SpawnStay10B: 10 figures growing out of the box (table $03A4DA: growth $266, energy $50, first word +$18, the mirror of ToyChain_MakeBox10A's -$18), sway oscillator $40/$55/$55/$78
; In:  a6 = the gift box (field $2A); inline dc.w $A, dc.l $03A4DA, dc.l $03A30E
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `000A 0003A4DA 0003A30E`; Op2_SpawnChild 002A at $039FE8; TAS f79928-f91325 (3 runs). The maker after it ($03A326, table $03A4F6, wide sway; its box $039C44 never runs) is still raw
; ---------------------------------------------------------------------------
ToyChain_MakeBox10B:
	jsr	ToyChain_SpawnInline(pc)
