; ---------------------------------------------------------------------------
; Hitbox_LinkByKind_AllPlayers - Hitbox_LinkByKind entry for kind 4: appends the box to g_PlayerBoxList ($8C3C), as kind 0
; In:  as Hitbox_LinkByKind
; Out: as Hitbox_LinkPlayerList
; Evidence: entry 4 of the jsr $4F3E(pc,d3.w) table, bra.w Hitbox_LinkPlayerList like entry 0; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Hitbox_LinkByKind_AllPlayers:
	bra.w	Hitbox_LinkPlayerList
