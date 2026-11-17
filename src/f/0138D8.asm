; ---------------------------------------------------------------------------
; Item_CollectRapidCharge - touch native of the rapid-charge pickup: sets the collecting player's rapid-charge timer +$26 to $2D0 frames
; In:  g_FoundBox = the toucher's box
; Out: player record +$26 = $2D0
; Evidence: op $2CB2 at $138D2 after sound $48; the player handler at $103A6 counts +$26 down and while it runs adds Tbl_WeaponChargeRate[type][0] = $280 per frame to the charge +$8 instead of the level's $80-$B8; 49 TAS frames (1556-128538)
; ---------------------------------------------------------------------------
Item_CollectRapidCharge:
	bsr.w	Item_FoundBoxPlayer
	move.w	#$2D0,$26(a0)
	rts
