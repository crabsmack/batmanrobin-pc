; ---------------------------------------------------------------------------
; Boss_Hatter_RoadHazardHit - hit callback of the road hazard's attack box: when Hitbox_CanHitTarget accepts the struck player box, pauses the hazard's attacks ($1(a4) = 15 frames), pushes it back one checker row ($42(a6) += $80) and knocks the player back (EnemyAtk_ApplyKnockSpeed: damage $20, knockback $300)
; In:  a0 = attack box, a2 = struck box, a4/a6 = the hazard
; Out: d0 = 0 hit / nonzero none
; Evidence: callback long $0003DE62 in the Op2_SetAttackBox spec at $03DE38 (kind 4, rect -21,-30..21,30, args $20/$300); ctarget fuzz-113400-s2
; ---------------------------------------------------------------------------
Boss_Hatter_RoadHazardHit:
	jsr	(Hitbox_CanHitTarget).l
	beq.w	loc_03DE6E
	rts
loc_03DE6E:
	move.b	#$F,$1(a4)
	addi.w	#$80,$42(a6)
	jmp	(EnemyAtk_ApplyKnockSpeed).l
; ---------------------------------------------------------------------------
; Boss_Hatter_RoadHazardPlace - per-frame placement of the road hazard: moves it toward the players by the road speed (distance $42 - g_SceneVars2+$C), finds its checker row in the road's row table (g_SceneVars+2 + 8 per row: line, line, x offset, scale), sets the drawn position $4E/$50 and the sprite priority (d6 $8000 unless its row is hidden), picks one of 24 perspective sizes (RAM frames $FC40 near .. $FDA8 far) by distance and adds the sprite; d0: 0 drawn, 1 drawn at the players' depth (attack box live), 2 passed (distance >= $8C0 unsigned: kill)
; In:  a6/a5 = the hazard
; Out: d0 = 0/1/2 (Op2_CallNativeRelSwitch)
; Evidence: Op2_CallNativeRelSwitch $03DE80 at $03DE28/$03DE58; frame words at $03DF14- read by `move.w $03DF04(pc,d7.w)`; Spr_AddBack when index > $13 (far) or $F-$13 (d0 = 1), Spr_AddFront when nearer; ctarget in 3 corpus runs (TAS f114753-)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadHazardPlace:
	move.w	$42(a6),d7
	sub.w	(g_SceneVars2+$C).w,d7
	cmpi.w	#$8C0,d7
	bcs.s	loc_03DE92
	moveq	#$2,d0
	rts
loc_03DE92:
	move.w	d7,$42(a6)
	addi.w	#$C0,d7
	move.w	d7,d0
	add.w	(g_SceneVars2+$E).w,d0
	lsr.w	#$4,d0
	andi.w	#$FFF8,d0
	lea.l	(g_SceneVars+$2).w,a1
	adda.w	d0,a1
	move.w	#$8000,d6
	move.w	(a1)+,d5
	cmp.w	(a1)+,d5
	ble.s	loc_03DEBA
	move.w	#$0,d6
loc_03DEBA:
	move.w	$3E(a6),d4
	add.w	(a1)+,d4
	muls.w	(a1),d4
	asl.l	#$4,d4
	swap	d4
	add.w	(g_SceneVars2+$3A).w,d4
	add.w	(g_SceneVars2+$3C).w,d5
	move.w	d4,$4E(a6)
	move.w	d5,$50(a6)
	subq.w	#$8,d7
	lsr.w	#$4,d7
	add.w	d7,d7
	move.w	loc_03DF0A-6(pc,d7.w),d1
	move.w	(a5),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a5)
	cmpi.w	#$26,d7
	ble.s	loc_03DEFA
	jsr	(Spr_AddBack).l
	moveq	#$0,d0
	rts
loc_03DEFA:
	cmpi.w	#$1E,d7
	blt.s	loc_03DF0A
	jsr	(Spr_AddBack).l
	moveq	#$1,d0
	rts
loc_03DF0A:
	jsr	(Spr_AddFront).l
	moveq	#$0,d0
	rts
