; ---------------------------------------------------------------------------
; Weapon_FireFlying - fires the player's weapon in the flying section: Weapon_Fire with the flying half ($C0) of Tbl_010AF8
; In:  as Weapon_Fire
; Out: as Weapon_Fire
; Evidence: native callback (a2 op $2CBC) at $4D05C and $4FB98; executed only in fuzz runs from frames 34200-79000 (the stage 2 flying section, g_PlayMode = 2)
; ---------------------------------------------------------------------------
Weapon_FireFlying:
	move.w	#$C0,d7
	bra.s	loc_010A86
; ---------------------------------------------------------------------------
; Weapon_Fire - fires the player's weapon: spawns the shot script for weapon type +$28 and level +$2A (charged variant when the charge word +$8 >= $2B00, which it then clears) at the player's position plus the script's offset, owned by id pad+2
; In:  a6 = player object (+$1E record, +$58 pad), a5 = sprite block (+$38 attr, bit 3 facing left), a2 = script operands: word dx (negated facing left), word dy, long d3 (high word facing right, low word facing left)
; Out: a2 past the 8 operand bytes; shot spawned through g_SpawnCtx and the a4 VM ($53BA)
; Evidence: native callback (a2 op $2CBC) from 50 sites in the player scripts ($4B6D6, $4C454 ...); Tbl_010AF8 holds a4 scripts $5034A-$504F0 (several shots at higher levels via op $609E); shot owner ids 3/4 score for P1/P2 (Op2_AwardScoreToHitter $A48); HUD_DrawWeaponBar shows the charge
; ---------------------------------------------------------------------------
Weapon_Fire:
	moveq	#$0,d7
loc_010A86:
	moveq	#$0,d2
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	(a2)+,d4
	move.w	(a2)+,d5
	move.l	(a2)+,d3
	btst	#$3,$38(a5)
	beq.s	loc_010A9E
	swap	d3
	neg.w	d4
loc_010A9E:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	clr.w	d3
	swap	d4
	swap	d5
	movea.l	$1E(a6),a0
	move.w	$28(a0),d0
	lsl.w	#$3,d0
	add.w	$2A(a0),d0
	lsl.w	#$3,d0
	add.w	d7,d0
	move.w	$8(a0),d1
	clr.w	$8(a0)
	cmpi.w	#$2B00,d1
	bcs.s	loc_010ACE
	addq.w	#$4,d0
loc_010ACE:
	movem.l	a2-a6,-(a7)
	movea.l	Tbl_010AF8(pc,d0.w),a4
	move.w	$58(a6),d0
	lea.l	(g_SpawnCtx).l,a6
	move.w	$38(a5),$22(a6)
	addq.w	#$2,d0
	move.w	d0,$10(a6)
	jsr	(VM4_Next).l
	movem.l	(a7)+,a2-a6
	rts
