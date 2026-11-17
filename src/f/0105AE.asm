; ---------------------------------------------------------------------------
; Player_ClearBits300 - clears bits $300 of one in-play player's script variable 1
; In:  a0 = player record
; Out: +$24(a4) &= $FCFF when record +$25 bit 0
; Evidence: tail of Player_ClearBits300All; TAS frame 33603
; ---------------------------------------------------------------------------
Player_ClearBits300:
	btst	#$0,$25(a0)
	beq.s	loc_0105C4
	movea.w	$2C(a0),a0
	movea.w	$A(a0),a0
	andi.w	#$FCFF,$24(a0)
loc_0105C4:
	rts
; ---------------------------------------------------------------------------
; EnemyAtk_HitKnockSpeed - enemy attack hit callback: when the target can be hit, damage +$34, knockback +$36 x 256 pushed toward screen middle near the edges, else against the attacker's x motion when that is faster than the knockback, else away from the attack; hit type 8, or 9 if the target is airborne
; In:  a0 = attack box (+$34 damage, +$36 knockback), a2 = struck box, a3/a5 = attacker blocks, d4-d7 = attack rectangle
; Out: d0 = 0 hit / nonzero none; box +$20 += damage, +$1A = 8/9, +$22 = knockback; spark Scr4_010964
; Evidence: Hitbox_CanHitTarget, then EnemyAtk_ApplyKnockSpeed; edges: box centre - g_CameraX <= $30 / >= $110 with 1.25x; $18(a3) compared with the knockback; type 9 when the owner's $54(a3) & $B = 0; box callback long at 52 sites ($18114, $28DC6, $35AD0 ...); 61 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitKnockSpeed:
	jsr	(Hitbox_CanHitTarget).l
	beq.s	EnemyAtk_ApplyKnockSpeed
	rts
; ---------------------------------------------------------------------------
; EnemyAtk_ApplyKnockSpeed - body of EnemyAtk_HitKnockSpeed after the hit test, also entered by enemy callbacks that do their own test and reaction
; In:  as EnemyAtk_HitKnockSpeed, hit already accepted
; Out: as EnemyAtk_HitKnockSpeed
; Evidence: jmp targets from $3B6FA and $3DE62 (after Hitbox_CanHitTarget and their own $1(a4) pause)
; ---------------------------------------------------------------------------
EnemyAtk_ApplyKnockSpeed:
	move.w	$36(a0),d0
	ext.l	d0
	asl.l	#$8,d0
	move.w	$12(a2),d2
	add.w	$16(a2),d2
	asr.w	#$1,d2
	sub.w	(g_CameraX).w,d2
	cmpi.w	#$30,d2
	ble.s	loc_0105F4
	cmpi.w	#$110,d2
	blt.s	loc_0105FC
	neg.l	d0
loc_0105F4:
	move.l	d0,d1
	asr.l	#$2,d1
	add.l	d1,d0
	bra.s	EnemyAtk_ApplyKnock
loc_0105FC:
	move.l	$18(a3),d2
	neg.l	d2
	bpl.s	loc_01060E
	neg.l	d0
	cmp.l	d0,d2
	blt.s	EnemyAtk_ApplyKnock
	neg.l	d0
	bra.s	EnemyAtk_KnockAwayFromBox
loc_01060E:
	cmp.l	d0,d2
	bgt.s	EnemyAtk_ApplyKnock
	bra.s	EnemyAtk_KnockAwayFromBox
; ---------------------------------------------------------------------------
; EnemyAtk_HitKnock - enemy attack hit callback like EnemyAtk_HitKnockSpeed, but in mid-screen the push is always away from the attack box (no attacker-speed case)
; In:  as EnemyAtk_HitKnockSpeed
; Out: as EnemyAtk_HitKnockSpeed
; Evidence: same edge test ($30/$110, 1.25x), then the box-centre vs d4+d6 comparison at $1064C; callback long at $1C54E, $1C562, $25420 and jsr from $1E768; 9 corpus runs
; ---------------------------------------------------------------------------
EnemyAtk_HitKnock:
	jsr	(Hitbox_CanHitTarget).l
	beq.w	loc_010620
	rts
loc_010620:
	move.w	$36(a0),d0
	ext.l	d0
	asl.l	#$8,d0
	move.w	$12(a2),d2
	add.w	$16(a2),d2
	asr.w	#$1,d2
	sub.w	(g_CameraX).w,d2
	cmpi.w	#$30,d2
	ble.s	loc_010644
	cmpi.w	#$110,d2
	blt.s	EnemyAtk_KnockAwayFromBox
	neg.l	d0
loc_010644:
	move.l	d0,d1
	asr.l	#$2,d1
	add.l	d1,d0
	bra.s	EnemyAtk_ApplyKnock
; ---------------------------------------------------------------------------
; EnemyAtk_KnockAwayFromBox - turns the knockback d0 to push the struck box away from the attack box (struck centre*2 - attack x1 - x2 < 0 negates), then EnemyAtk_ApplyKnock
; In:  d0 = knockback (16.16), a2 = struck box, d4/d6 = attack box x1/x2
; Out: as EnemyAtk_ApplyKnock
; Evidence: branch target of EnemyAtk_HitKnock (mid-screen) and EnemyAtk_ApplyKnockSpeed (attacker slower than the knockback); falls into EnemyAtk_ApplyKnock
; ---------------------------------------------------------------------------
EnemyAtk_KnockAwayFromBox:
	move.w	$12(a2),d2
	add.w	$16(a2),d2
	sub.w	d4,d2
	sub.w	d6,d2
	bpl.s	EnemyAtk_ApplyKnock
	neg.l	d0
; ---------------------------------------------------------------------------
; EnemyAtk_ApplyKnock - common tail of the knockback hit callbacks: damage +$34 to the struck box, hit type 8 (9 when the target's motion contact bits $B are clear: airborne), knockback d0, spark Scr4_010964, d0 = 0
; In:  d0 = knockback (16.16), a0 = attack box, a2 = struck box, d4-d7 = attack rectangle
; Out: d0 = 0; box +$20/+$1A/+$22
; Evidence: branch target of EnemyAtk_ApplyKnockSpeed, EnemyAtk_HitKnock, EnemyAtk_KnockAwayFromBox; bsr Hitbox_SpawnAtContact / moveq #0,d0 / rts
; ---------------------------------------------------------------------------
EnemyAtk_ApplyKnock:
	move.w	$34(a0),d1
	add.w	d1,$20(a2)
	movea.w	$2E(a2),a1
	movea.w	$8(a1),a1
	move.w	#$8,d1
	move.w	$54(a1),d2
	andi.w	#$B,d2
	bne.s	loc_01067C
	moveq	#$9,d1
loc_01067C:
	move.w	d1,$1A(a2)
	move.l	d0,$22(a2)
	lea.l	(Scr4_010964).l,a1
	bsr.w	Hitbox_SpawnAtContact
	moveq	#$0,d0
	rts
