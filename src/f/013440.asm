; ---------------------------------------------------------------------------
; Item_CollectBigHealth - touch native of the large health pickup: gives the collecting player $80 health (half the bar), capped at $100
; In:  g_FoundBox = the box that touched the pickup
; Out: player record +4 raised; returns early (Item_FoundBoxPlayer) when the toucher is not a player
; Evidence: op $2CB2 at $1343A right after sound $41; +4 = health ($100 full); 7 TAS frames (7565-71935), 6 corpus runs
; ---------------------------------------------------------------------------
Item_CollectBigHealth:
	bsr.w	Item_FoundBoxPlayer
	addi.w	#$80,$4(a0)
	cmpi.w	#$100,$4(a0)
	ble.s	loc_013458
	move.w	#$100,$4(a0)
loc_013458:
	rts
