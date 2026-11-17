; ---------------------------------------------------------------------------
; Hitbox_CanHitTarget - hit test for enemy attacks: Hitbox_CanHitPlayer extended to owner id 5: accepts a struck box owned by id 5, 1 or 2 whose owner is not invulnerable and which was not struck yet
; In:  a2 = struck collision box (+$10 owner id)
; Out: d0 = 0 (Z) = hit accepted; nonzero otherwise
; Evidence: cmpi.w #5 / beq to Hitbox_CanHitPlayer's owner checks ($107FA), else falls into Hitbox_CanHitPlayer's 1/2 test; what owns id 5 is not identified (set by Op2_SetOwnerId); callers the EnemyAtk_* callbacks, $3B6FA, $3DE62, $41FFC; 151 corpus runs
; ---------------------------------------------------------------------------
Hitbox_CanHitTarget:
	move.w	$10(a2),d0
	cmpi.w	#$5,d0
	beq.s	Hitbox_CanHitOwner
