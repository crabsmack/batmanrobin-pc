; ---------------------------------------------------------------------------
; Enemy_ArenaDebris_HitPlayer - attack-box callback of the debris that tumbles into the cat-head arena: when the target can be hit, pauses the debris's own attacks for 100 frames ($1(a4) = $64), shakes its sprite 8 frames, turns a fall into a bounce at a quarter of the speed, then applies the knockback hit
; In:  a0 = attack box (+$34 damage $20, +$36 knockback $200), a2 = struck box, a3/a4/a5 = the debris's blocks
; Out: d0 = 0 hit / nonzero none (EnemyAtk_ApplyKnockSpeed)
; Evidence: box spec `... 0003B6FA 0020 0200` at $03B614 in the debris body $03B5EC; Hitbox_CanHitTarget then jmp EnemyAtk_ApplyKnockSpeed; TAS f88206-f88462 (16 frames)
; ---------------------------------------------------------------------------
Enemy_ArenaDebris_HitPlayer:
	jsr	(Hitbox_CanHitTarget).l
	beq.w	loc_03B706
	rts
loc_03B706:
	move.b	#$64,$1(a4)
	move.b	#$8,$17(a5)
	move.l	$1C(a3),d0
	bmi.s	loc_03B720
	asr.l	#$2,d0
	neg.l	d0
	move.l	d0,$1C(a3)
loc_03B720:
	jmp	(EnemyAtk_ApplyKnockSpeed).l
