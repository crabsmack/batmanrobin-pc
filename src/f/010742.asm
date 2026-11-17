; ---------------------------------------------------------------------------
; EnemyAtk_DamageFoundBox - a2 native: damages the box found by the last overlap op (g_FoundBox) by the script word, as a flinch (hit type 6), if it can be hit
; In:  a2 = script (word operand: damage)
; Out: a2 past the operand; box +$20 += damage, +$1A = 6
; Evidence: move.w (a2)+,d7; movea.l g_FoundBox,a2; Hitbox_CanHitTarget; dc.l after op $2CBC at 12 sites ($1E082, $1F51C ... $284B4); 63 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_DamageFoundBox:
	move.w	(a2)+,d7
	move.l	a2,-(a7)
	movea.l	(g_FoundBox).w,a2
	bsr.w	Hitbox_CanHitTarget
	bne.s	loc_01075A
	add.w	d7,$20(a2)
	move.w	#$6,$1A(a2)
loc_01075A:
	movea.l	(a7)+,a2
	rts
; ---------------------------------------------------------------------------
; EnemyAtk_HitPlayerA - enemy attack hit callback for players only: Hitbox_CanHitPlayer, then EnemyAtk_ApplyHit with the spark Scr4_010940 (frames $221A-$222E)
; In:  as EnemyAtk_ApplyHit
; Out: as EnemyAtk_ApplyHit; rts with d0 != 0 when no hit
; Evidence: callback long at $1BE3A, $251D4, $252AE; 14 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitPlayerA:
	bsr.w	Hitbox_CanHitPlayer
	bne.s	loc_01076E
	lea.l	(Scr4_010940).l,a1
	bra.w	EnemyAtk_ApplyHit
loc_01076E:
	rts
; ---------------------------------------------------------------------------
; EnemyAtk_HitPlayerB - enemy attack hit callback for players only, with the spark Scr4_01091A (frames $226A-$227E)
; In:  as EnemyAtk_ApplyHit
; Out: as EnemyAtk_ApplyHit; rts with d0 != 0 when no hit
; Evidence: callback long at $1B4EE, $1BD80, $1BDD4; 12 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitPlayerB:
	bsr.w	Hitbox_CanHitPlayer
	bne.s	loc_010780
	lea.l	(Scr4_01091A).l,a1
	bra.w	EnemyAtk_ApplyHit
loc_010780:
	rts
