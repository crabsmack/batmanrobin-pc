; ---------------------------------------------------------------------------
; Boot_InitIOPorts - after the region check passes: waits for any VDP DMA to finish, clears the three serial-control registers, sets the pad/expansion port control registers to $40 (TH as output) and jumps into the game's main init at $9E02
; In:  none (from Boot_CheckRegion)
; Out: never returns (jmp Main_Start)
; Evidence: polls $C00004 bit 1 (DMA busy); writes $00 to $A10013/$A10019/$A1001F and $40 to $A10009/$A1000B/$A1000D; runs once on TAS frame 3
; ---------------------------------------------------------------------------
Boot_InitIOPorts:
	move.w	($C00004).l,d0
	btst	#$1,d0
	bne.s	Boot_InitIOPorts
	move.b	#$0,($A10013).l
	move.b	#$0,($A10019).l
	move.b	#$0,($A1001F).l
	move.b	#$40,($A10009).l
	move.b	#$40,($A1000B).l
	move.b	#$40,($A1000D).l
	jmp	(Main_Start).l
