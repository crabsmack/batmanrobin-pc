; ---------------------------------------------------------------------------
; EnemyAtk_ApplyHit - common tail of EnemyAtk_HitPlayerA/B: damage +$34 to the box, knockback +$36 x 256 by the attacker's facing, hit type 7, a 7-frame movement pause for the attacker and the spark a1
; In:  a0 = attack box, a1 = spark script, a2 = struck box, a3/a5 = attacker blocks, d4-d7 = attack rectangle
; Out: box +$20 += damage, +$22 = knockback, +$1A = 7; $1(a3) = 7; falls into Hitbox_SpawnAtContact
; Evidence: negated when $38(a5) bit 3 (facing left), as PlayerAtk_ApplyHit; 25 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_ApplyHit:
	move.w	$34(a0),d0
	add.w	d0,$20(a2)
	move.w	$36(a0),d0
	ext.l	d0
	asl.l	#$8,d0
	btst	#$3,$38(a5)
	beq.s	loc_0107CA
	neg.l	d0
loc_0107CA:
	move.l	d0,$22(a2)
	move.b	#$7,$1(a3)
	move.w	#$7,$1A(a2)
	bra.w	Hitbox_SpawnAtContact
