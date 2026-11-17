; ---------------------------------------------------------------------------
; Item_FoundBoxPlayer - pickup helper: the player record of whoever owns g_FoundBox (the box the pickup touched); returns to the caller's caller when it is not a player
; In:  none (g_FoundBox)
; Out: a0 = g_Player1 or g_Player2, a1 = the box owner
; Evidence: first call of the item-collect natives $013440 (health +$80), $0135F4 (lives +1), $01375E, $0138D8, $013C8A; 126 TAS frames (603-128551)
; ---------------------------------------------------------------------------
Item_FoundBoxPlayer:
	movea.l	(g_FoundBox).w,a1
