; ---------------------------------------------------------------------------
; Item_CollectBomb - touch native of the bomb pickup: starts the 15-frame screen-clear pause and deals $200 damage to every enemy box, credited to the collecting player
; In:  g_FoundBox = the toucher's box
; Out: g_ScreenClearTimer = $F; every enemy box struck (Hitbox_DamageAllEnemies, d1 = the player's pad +$3E as attacker id)
; Evidence: op $2CB2 at $1377A after sound $10; the script then calls Pal_FlashWhite ($9584) and the shake $95A2; same effect as Hitbox_WipeEnemies; 45 TAS frames (1299-128551)
; ---------------------------------------------------------------------------
Item_CollectBomb:
	bsr.w	Item_FoundBoxPlayer
	move.w	$3E(a0),d1
	move.w	#$F,(g_ScreenClearTimer).w
	move.w	#$200,d0
	jmp	(Hitbox_DamageAllEnemies).l
