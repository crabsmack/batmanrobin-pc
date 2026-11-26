; ---------------------------------------------------------------------------
; Hitbox_KindAllEnemies - entry $C of Hitbox_TestAttackBoxAt's kind jump table: attack-box kind $C goes to Hitbox_HitAllEnemyBoxes
; In:  as Hitbox_TestAttackBoxAt
; Out: as Hitbox_HitAllEnemyBoxes
; Evidence: the jmp $4E22(pc,d3.w) table after Hitbox_KindFirstEnemy; never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Hitbox_KindAllEnemies:
	bra.w	Hitbox_HitAllEnemyBoxes
