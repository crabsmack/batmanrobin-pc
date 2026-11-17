; ---------------------------------------------------------------------------
; Item_CollectWeaponLevelUp - power-up collect native: raises the collecting player's weapon level (+$2A of the player record) by one, not past 6
; In:  none (g_FoundBox, through Item_FoundBoxPlayer)
; Out: player record +$2A level
; Evidence: bsr Item_FoundBoxPlayer, cmpi.w #6 / addq.w #1 on $2A(a0) (NOTES Weapons: level 0-6); the native before the a2 script at $13D2A; run only on the stepped runner (P4 kickoff 2026-10-27, 5 fuzz runs)
; ---------------------------------------------------------------------------
Item_CollectWeaponLevelUp:
	bsr.w	Item_FoundBoxPlayer
	cmpi.w	#$6,$2A(a0)
	beq.s	loc_013D28
	addq.w	#$1,$2A(a0)
loc_013D28:
	rts
