; ---------------------------------------------------------------------------
; Hitbox_HitFirstPlayerBox - attack-box kind 0: strikes the first box on g_PlayerBoxList that overlaps the rectangle, if any
; In:  d4-d7 = rectangle; a0 = attack box; a6 = owner
; Out: Hitbox_Strike on a hit, else rts
; Evidence: bsr Hitbox_FindPlayerBox / bne Hitbox_Strike; entry 0 of the jump table in Hitbox_TestAttackBoxAt; 32 corpus runs
; ---------------------------------------------------------------------------
Hitbox_HitFirstPlayerBox:
	bsr.w	Hitbox_FindPlayerBox
	bne.s	Hitbox_Strike
	rts
; ---------------------------------------------------------------------------
; Hitbox_HitAllPlayerBoxes - attack-box kind 4: strikes every box on g_PlayerBoxList that overlaps the rectangle
; In:  d4-d7 = rectangle; a0 = attack box; a6 = owner
; Out: Hitbox_StrikeAll on a hit, else rts
; Evidence: bsr Hitbox_FindPlayerBox / bne Hitbox_StrikeAll; jump-table entry 4 ($4E26)
; ---------------------------------------------------------------------------
Hitbox_HitAllPlayerBoxes:
	bsr.w	Hitbox_FindPlayerBox
	bne.s	Hitbox_StrikeAll
	rts
; ---------------------------------------------------------------------------
; Hitbox_HitFirstEnemyBox - attack-box kind 8: strikes the first box on g_EnemyBoxList that overlaps the rectangle (the players' attacks)
; In:  d4-d7 = rectangle; a0 = attack box; a6 = owner
; Out: Hitbox_Strike on a hit, else rts
; Evidence: bsr Hitbox_FindEnemyBox / bne Hitbox_Strike; jump-table entry 8 ($4E2A)
; ---------------------------------------------------------------------------
Hitbox_HitFirstEnemyBox:
	bsr.w	Hitbox_FindEnemyBox
	bne.s	Hitbox_Strike
	rts
