; ---------------------------------------------------------------------------
; ToyChain_MakeChild7 - inline chain maker of ToyChain_Spawn7: a 7-figure toy chain (table Tbl_039A0A+$BA, energy $90) on the chain body Scr2_0379DA, random speed $4C-$100, sway (x) cruise $200
; In:  a6 = the base object (ToyChain_Spawn7, script Scr2_0397EE) whose field $2A gets it; the 10 inline bytes after the jsr: dc.w 7 links, dc.l table, dc.l script $03990C
; Out: d0 = 0 and a new unlinked 5-block boss-class chain object, or nonzero when the pool is full (ToyChain_SpawnInline)
; Evidence: jsr ToyChain_SpawnInline + `0007 00039AC4 0003990C`; `244A 002A 000398FE` (Op2_SpawnChild) in Scr2_0397EE; TAS f85421 (section $3100 backlot); png shot f85500: a leaning column of 7 toy figures (frames $4344-$43DA)
; ---------------------------------------------------------------------------
ToyChain_MakeChild7:
	jsr	ToyChain_SpawnInline(pc)
