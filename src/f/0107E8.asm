; ---------------------------------------------------------------------------
; Hitbox_CanHitPlayer - hit test for attacks on players: accepts the struck box a2 when its owner id is 1 or 2 (a player), that player's invulnerability byte $16 of its a5 block is 0, and this box was not yet struck (sets box+$3C bit 0)
; In:  a2 = struck collision box (+$10 owner id, +$2E owner object)
; Out: d0 = 0 (Z) = hit accepted; nonzero otherwise; d1 = the owner's $58
; Evidence: called by the enemy hit callbacks $1075E/$10770 (referenced from enemy scripts $1BE3A, $251D4, $1B4EE ...); the player object $101CE sets $16(a5) = $78 on (re)spawn; box +$3C bit 0 is the struck flag (g_StruckBox); runs in 154 corpus runs
; ---------------------------------------------------------------------------
Hitbox_CanHitPlayer:
	move.w	$10(a2),d0
	cmpi.w	#$1,d0
	beq.s	Hitbox_CanHitOwner
	cmpi.w	#$2,d0
	beq.s	Hitbox_CanHitOwner
	rts
; ---------------------------------------------------------------------------
; Hitbox_CanHitOwner - the owner half of the enemy-attack hit tests: accepts box a2 when its owner's sprite block +$16 (invulnerability) is 0 and the box's struck bit (+$3C bit 0) was clear, setting it
; In:  a2 = struck box (+$2E owner)
; Out: d0 = 0 (Z) accepted, else unchanged (the owner id, Z clear); d1 = the owner's $58; a1 = its sprite block
; Evidence: branch target of Hitbox_CanHitPlayer (ids 1/2) and Hitbox_CanHitTarget (id 5)
; ---------------------------------------------------------------------------
Hitbox_CanHitOwner:
	movea.w	$2E(a2),a1
	move.w	$58(a1),d1
	movea.w	$C(a1),a1
	tst.b	$16(a1)
	bne.s	loc_010816
	bset	#$0,$3C(a2)
	bne.s	loc_010816
	moveq	#$0,d0
loc_010816:
	rts
; ---------------------------------------------------------------------------
; PlayerAtk_HitPause - player attack hit callback: sound $A, pauses the attacker's script 8 frames ($1(a4)) and its movement 6 frames ($1(a3)), then PlayerAtk_ApplyHit with spark Scr4_0108F4
; In:  a0 = attack box (+$34 damage, +$36 knockback), a2 = struck box, a3/a4/a5 = the attacker's motion/script/sprite blocks, d4-d7 = attack rectangle
; Out: as PlayerAtk_ApplyHit
; Evidence: box callback long (+$30, set by the a2 op $3784) in the Batman ($4B824, $4B886) and Robin ($4E360, $4E3C2) scripts; $1(a4) is the script delay counter ($4DA4), $1(a3) zeroes motion while set ($4B74); executed in 21 fuzz runs
; ---------------------------------------------------------------------------
PlayerAtk_HitPause:
	move.w	#$A,d0
	jsr	(Snd_PlaySfx).l
	move.b	#$8,$1(a4)
	move.b	#$6,$1(a3)
	lea.l	(Scr4_0108F4).l,a1
	bra.s	PlayerAtk_ApplyHit
; ---------------------------------------------------------------------------
; PlayerAtk_HitLongPause - player attack hit callback: sound $A, pauses the attacker's script 12 frames, then PlayerAtk_ApplyHit with spark Scr4_0108CE
; In:  as PlayerAtk_HitPause
; Out: as PlayerAtk_ApplyHit
; Evidence: box callback in 5 Batman ($4C1CA-$4C39A) and 5 Robin ($4ED06-$4EED6) attack definitions; executed in 66 corpus runs
; ---------------------------------------------------------------------------
PlayerAtk_HitLongPause:
	move.w	#$A,d0
	jsr	(Snd_PlaySfx).l
	move.b	#$C,$1(a4)
	lea.l	(Scr4_0108CE).l,a1
; ---------------------------------------------------------------------------
; PlayerAtk_ApplyHit - common tail of the player hit callbacks: marks the box struck, moves the attack damage from its energy +$1E to damage taken +$20, sets its knockback +$22 by the attacker's facing, pushes an airborne attacker back (unless contact bit 3), and spawns the spark at the contact point
; In:  a0 = attack box, a1 = spark script, a2 = struck box, a3/a5 = attacker blocks, d4-d7 = attack rectangle
; Out: falls into Hitbox_SpawnAtContact
; Evidence: branch target of PlayerAtk_HitPause, PlayerAtk_HitLongPause; $36(a0) << 8 gives the knockback, negated when $38(a5) bit 3 (facing left); $2(a3) = -/+1.0 unless $54(a3) bit 3
; ---------------------------------------------------------------------------
PlayerAtk_ApplyHit:
	bset	#$0,$3C(a2)
	move.w	$34(a0),d0
	add.w	d0,$20(a2)
	sub.w	d0,$1E(a2)
	move.w	$36(a0),d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	#$FFFF0000,d1
	btst	#$3,$38(a5)
	beq.s	loc_010878
	neg.l	d0
	neg.l	d1
loc_010878:
	move.l	d0,$22(a2)
	move.w	$54(a3),d0
	andi.w	#$8,d0
	bne.s	Hitbox_SpawnAtContact
	move.l	d1,$2(a3)
