; ---------------------------------------------------------------------------
; Hitbox_LinkByKind_Enemy - Hitbox_LinkByKind entry for kind 8: appends the box to g_EnemyBoxList ($8C54)
; In:  a0 = box
; Out: box linked
; Evidence: bra.w Hitbox_LinkEnemyList; reached by jsr $4F3E(pc,d3.w) with d3 = 8 (138 corpus runs)
; ---------------------------------------------------------------------------
Hitbox_LinkByKind_Enemy:
	bra.w	Hitbox_LinkEnemyList
