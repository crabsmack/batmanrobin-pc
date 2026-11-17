; ---------------------------------------------------------------------------
; Item_CollectSmallHealth - touch native of the small health pickup: gives the collecting player $10 health (1/16 of the bar), capped at $100
; In:  none (g_FoundBox, through Item_FoundBoxPlayer)
; Out: player record +4 health
; Evidence: Op2_CallNativeRel at $01353A in the small pickup's body $1351E (sound $12); bsr Item_FoundBoxPlayer / addi.w #$10,4(a0) / cap $100: Item_CollectBigHealth's shape with $10; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Item_CollectSmallHealth:
	bsr.w	Item_FoundBoxPlayer
	addi.w	#$10,$4(a0)
	cmpi.w	#$100,$4(a0)
	ble.s	loc_013558
	move.w	#$100,$4(a0)
loc_013558:
	rts
