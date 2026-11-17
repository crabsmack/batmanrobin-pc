; ---------------------------------------------------------------------------
; Pad_CheckStart - a2 native test: reports whether Start was just pressed on pad 1 or 2 and records which pad
; In:  pad records $F624/$F628 (+2 = newly pressed)
; Out: d0 = 1 and g_StartPad = 1/2 when bit 7 of $F626/$F62A is set, else d0 = 0
; Evidence: called by op $2D40 (`dc.l $A196`) in the continue screen script at $491F2; ran only in scenario-continue and scenario-game-over
; ---------------------------------------------------------------------------
Pad_CheckStart:
	btst	#$7,($FFFFF626).w
	beq.s	loc_00A1A8
	move.b	#$1,(g_StartPad).w
	moveq	#$1,d0
	rts
loc_00A1A8:
	btst	#$7,($FFFFF62A).w
	beq.s	loc_00A1BA
	move.b	#$2,(g_StartPad).w
	moveq	#$1,d0
	rts
loc_00A1BA:
	moveq	#$0,d0
	rts
