; ---------------------------------------------------------------------------
; Weapon_KeepShotArt - keeps both players' current weapon-shot art resident in VRAM (Weapon_KeepShotArtOne for g_Player1 and g_Player2)
; In:  none
; Out: g_ShotArtSlots updated
; Evidence: called by the frame start $9B86 on 122886 TAS frames; slots $EA0E (P1) and $EA14 (P2)
; ---------------------------------------------------------------------------
Weapon_KeepShotArt:
	lea.l	(g_Player1).l,a6
	lea.l	(g_ShotArtSlots).l,a5
	bsr.s	Weapon_KeepShotArtOne
	lea.l	$40(a6),a6
	lea.l	$6(a5),a5
