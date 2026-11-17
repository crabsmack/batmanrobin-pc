; ---------------------------------------------------------------------------
; Player_HurtFixed - a2 native: takes $20 (1/8 of full) health from the player without a hit; at 0 it plays sound request 1 and loses a life, and removes the player when none are left
; In:  a6 = player object (+$1E = record)
; Out: record +4 reduced; on the last life falls into Player_DespawnNative
; Evidence: subi.w #$20,4(a0); Player_LoseLife d0 != 0 -> $102C8; dc.l after op $2CBC at $4AD4E (Batman) / $4D88A (Robin); cause not identified (TAS frames 92483-92850, stage 3)
; ---------------------------------------------------------------------------
Player_HurtFixed:
	movea.l	$1E(a6),a0
	subi.w	#$20,$4(a0)
	bgt.s	loc_010422
	move.w	#$1,d0
	jsr	(Snd_PlaySfx).l
	clr.w	$4(a0)
	bsr.w	Player_LoseLife
	beq.s	loc_010422
	bra.w	Player_DespawnNative
loc_010422:
	rts
; ---------------------------------------------------------------------------
; Player_TakeHit - a2 switch native: applies the hit recorded in g_StruckBox to the player: health -= the damage taken, then invulnerability and a reaction by hit type (6 flinch 30 frames; 7 knockback 40; 8 knockback 60; 9 knockdown 60), or death
; In:  a6/a3/a5 = player object and blocks; g_StruckBox = the player's struck box (+$1A hit type, +$20 damage, +$22 knockback)
; Out: d0 = case: 0 no reaction (also while invulnerable, $16(a5) != 0), 1 knockback, 2 knockdown, 3 dead (health 0, sound request 1); $16(a5) = invulnerable frames; knockback long to $2(a3) for types 7-9
; Evidence: dc.l after op $2D40 (Op2_CallNativeSwitch) at 41 sites from $4AE50 in the player scripts; the hit callbacks set +$1A to 6/7/8/9 (EnemyAtk_*); 150 corpus runs
; ---------------------------------------------------------------------------
Player_TakeHit:
	moveq	#$0,d0
	tst.b	$16(a5)
	bne.s	loc_01048C
	movea.l	(g_StruckBox).w,a0
	move.w	$1A(a0),d2
	moveq	#$1E,d1
	cmpi.w	#$6,d2
	beq.s	loc_010456
	move.l	$22(a0),$2(a3)
	moveq	#$28,d1
	moveq	#$1,d0
	cmpi.w	#$7,d2
	beq.s	loc_010456
	moveq	#$3C,d1
	cmpi.w	#$8,d2
	beq.s	loc_010456
	moveq	#$2,d0
loc_010456:
	movea.l	$1E(a6),a1
	move.w	$20(a0),d2
	clr.w	$20(a0)
	sub.w	d2,$4(a1)
	bpl.s	loc_01047A
	clr.w	$4(a1)
	move.w	#$1,d0
	jsr	(Snd_PlaySfx).l
	moveq	#$3,d0
	rts
loc_01047A:
	move.b	d1,$16(a5)
	move.w	d0,d7
	move.w	#$0,d0
	jsr	(Snd_PlaySfx).l
	move.w	d7,d0
loc_01048C:
	rts
