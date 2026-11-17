; ---------------------------------------------------------------------------
; Hitbox_TestAttackBox - tests one attack box at the object's position: Hitbox_TestAttackBoxAt with d1/d2 = x/y $3E/$42(a6)
; In:  a0 = attack box (pool block: +6 kind, rect left/top/right/bottom relative); a5 flip bits; a6 = owner
; Out: as Hitbox_TestAttackBoxAt
; Evidence: move.w $3E(a6),d1 / move.w $42(a6),d2 and falls in; called by Op2_TestAttackBoxes; 22k TAS frames
; ---------------------------------------------------------------------------
Hitbox_TestAttackBox:
	move.w	$3E(a6),d1
	move.w	$42(a6),d2
