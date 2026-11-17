; ---------------------------------------------------------------------------
; EnemyAtk_HitTouch - enemy body-contact hit callback: when the target can be hit, adds the attacker's touch damage $12(a6) and a flinch (hit type 6), no spark
; In:  a6 = attacker object (+$12 touch damage), a2 = struck box
; Out: d0 = 0 hit / nonzero none; box +$20 += damage, +$1A = 6
; Evidence: Hitbox_CanHitTarget; referenced at 21 sites ($1506C-$151C6, and after the a4 native op $5438 at $1ABAE, $1B568 ...); 81 corpus runs incl. the TAS (frames 2457-125837)
; ---------------------------------------------------------------------------
EnemyAtk_HitTouch:
	bsr.w	Hitbox_CanHitTarget
	bne.s	loc_0106BE
	move.w	$12(a6),d0
	add.w	d0,$20(a2)
	move.w	#$6,$1A(a2)
	moveq	#$0,d0
loc_0106BE:
	rts
; ---------------------------------------------------------------------------
; EnemyAtk_HitTouchSpark - EnemyAtk_HitTouch plus the hit spark Scr4_010964 at the contact point
; In:  as EnemyAtk_HitTouch, d4-d7 = attack rectangle
; Out: as EnemyAtk_HitTouch
; Evidence: Hitbox_SpawnAtContact with Scr4_010964; callback long at $3029A, $359FC and jsr from $42FA6; 11 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitTouchSpark:
	bsr.w	Hitbox_CanHitTarget
	bne.s	loc_0106E0
	move.w	$12(a6),d0
	add.w	d0,$20(a2)
	move.w	#$6,$1A(a2)
	lea.l	(Scr4_010964).l,a1
	bsr.w	Hitbox_SpawnAtContact
	moveq	#$0,d0
loc_0106E0:
	rts
; ---------------------------------------------------------------------------
; EnemyAtk_HitKnockdown3 - enemy hit callback: touch damage $12(a6), knockdown (hit type 9) with a 3.0 push away from the attack, spark Scr4_010964
; In:  as EnemyAtk_HitTouchSpark
; Out: d0 = 0 hit / nonzero none; box +$22 = +-$30000
; Evidence: d2 = $30000 then the shared tail $106F0 (sign from box centre vs d4+d6); callback long at $26B9A, $3B84A; 4 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitKnockdown3:
	move.l	#$30000,d2
	bra.s	loc_0106F0
; ---------------------------------------------------------------------------
; EnemyAtk_HitKnockdown4 - EnemyAtk_HitKnockdown3 with a 4.0 push
; In:  as EnemyAtk_HitTouchSpark
; Out: box +$22 = +-$40000
; Evidence: d2 = $40000; callback long at $26D08, $26D72, $363D6, $42380, $4349E; 24 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitKnockdown4:
	move.l	#$40000,d2
loc_0106F0:
	bsr.w	Hitbox_CanHitTarget
	beq.s	loc_0106F8
	rts
loc_0106F8:
	move.w	$12(a6),d0
	lea.l	(Scr4_010964).l,a1
	add.w	d0,$20(a2)
	move.w	#$9,$1A(a2)
	move.w	$12(a2),d1
	add.w	$16(a2),d1
	sub.w	d4,d1
	sub.w	d6,d1
	bpl.s	loc_01071C
	neg.l	d2
loc_01071C:
	move.l	d2,$22(a2)
	bra.w	Hitbox_SpawnAtContact
