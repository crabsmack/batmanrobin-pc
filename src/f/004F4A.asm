; ---------------------------------------------------------------------------
; Hitbox_LinkByKind_AllEnemies - Hitbox_LinkByKind entry for kind $C: appends the box to g_EnemyBoxList ($8C54), as kind 8
; In:  as Hitbox_LinkByKind
; Out: as Hitbox_LinkEnemyList
; Evidence: entry $C of the jsr $4F3E(pc,d3.w) table, bra.w Hitbox_LinkEnemyList like Hitbox_LinkByKind_Enemy; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Hitbox_LinkByKind_AllEnemies:
	bra.w	Hitbox_LinkEnemyList
