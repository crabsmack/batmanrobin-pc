; ---------------------------------------------------------------------------
; Hitbox_HitAllEnemyBoxes - attack-box kind $C: strikes every box on g_EnemyBoxList that overlaps the rectangle
; In:  d4-d7 = the rectangle (as Hitbox_HitAllPlayerBoxes)
; Out: as Hitbox_StrikeAll; rts when none overlaps
; Evidence: bsr Hitbox_FindEnemyBox / bne Hitbox_StrikeAll / rts, Hitbox_HitAllPlayerBoxes' shape on the enemy list; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Hitbox_HitAllEnemyBoxes:
	bsr.w	Hitbox_FindEnemyBox
	bne.s	Hitbox_StrikeAll
	rts
