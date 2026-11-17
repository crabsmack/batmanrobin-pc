; ---------------------------------------------------------------------------
; ToyChain_MakeBox1 - inline chain maker of a single toy figure on a stalk for the small gift boxes (table $03A4BE: 1 link, growth $19, energy $100), on body $037DD6 with an oscillator but no sway set
; In:  a6 = the gift box (field $2A); inline dc.w 1, dc.l $03A4BE, dc.l $03A2F4
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: jsr ToyChain_SpawnInline + `0001 0003A4BE 0003A2F4`; Op2_SpawnChild 002A in six gift-box scripts ($039DA2, $039E10, $039E6E, $039ECC, $039F2C, $039F8C); TAS f91509-f112294 (8 runs); level-skip scenario f10469
; ---------------------------------------------------------------------------
ToyChain_MakeBox1:
	jsr	ToyChain_SpawnInline(pc)
