; ---------------------------------------------------------------------------
; Item_WeaponItemGone - a weapon or power-up pickup ends (collected, or its off-screen exit $13B9A/$13BF0): one fewer counted in g_WeaponItemCount, not below 0
; In:  none
; Out: g_WeaponItemCount - 1 (floor 0)
; Evidence: native (op $2CB2) at $13C48/$13C56 just before Op2_Kill; pairs with the increment in Item_WeaponItemInit; 67 TAS frames (603-127253), 43 runs
; ---------------------------------------------------------------------------
Item_WeaponItemGone:
	subq.w	#$1,(g_WeaponItemCount).w
	bcc.s	loc_013C94
	clr.w	(g_WeaponItemCount).w
loc_013C94:
	rts
; ---------------------------------------------------------------------------
; Item_WeaponItemInit - weapon pickup setup native: counts the pickup in g_WeaponItemCount and picks its value at random from a value table (count word, values, then a cycle that loops on a negative offset)
; In:  a2 = script at a word offset to the value table; a6 = the pickup
; Out: $2A(a6) = $2C(a6) = the value (0 = level down, 1-3 = weapon type 0-2); $1E(a6) = the cycle pointer; a2 past the operand
; Evidence: op $2CB2 in all 20 weapon spawn scripts ($1394C-$13B62) with tables $13C5C-$13C7E; Rand_Below(count) selects the entry; inferred from the code (interior entry, not tracked per frame)
; ---------------------------------------------------------------------------
Item_WeaponItemInit:
	addq.w	#$1,(g_WeaponItemCount).w
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.w	(a0)+,d0
	jsr	(Rand_Below).l
	add.w	d0,d0
	adda.w	d0,a0
	move.w	(a0)+,d0
	move.w	d0,$2A(a6)
	move.w	d0,$2C(a6)
	move.l	a0,$1E(a6)
	rts
; ---------------------------------------------------------------------------
; Item_WeaponItemNextValue - weapon pickup native: steps the value cycle and stores the next value in $2C(a6) (a negative word rewinds the cycle)
; In:  a6 = the pickup ($1E cycle pointer)
; Out: $2C(a6) = next value, $1E(a6) advanced
; Evidence: op $2CB2 at $13C28 (every $5A0 ticks, spawn-action pickups) and $13C36 (every $60 ticks, power-ups); code read, not traced
; ---------------------------------------------------------------------------
Item_WeaponItemNextValue:
	movea.l	$1E(a6),a0
	move.w	(a0)+,d0
	bpl.s	loc_013CC6
	adda.w	d0,a0
	move.w	(a0)+,d0
loc_013CC6:
	move.w	d0,$2C(a6)
	move.l	a0,$1E(a6)
	rts
; ---------------------------------------------------------------------------
; Item_WeaponItemValue - weapon pickup native for the op $2D30 switch: returns the pickup's current value so the script picks the matching icon animation
; In:  a6 = the pickup
; Out: d0 = $2A(a6)
; Evidence: target of nine op $2D30 switches in the icon scripts $13D42-$13DC2; code read
; ---------------------------------------------------------------------------
Item_WeaponItemValue:
	move.w	$2A(a6),d0
	rts
; ---------------------------------------------------------------------------
; Item_WeaponItemShowNext - weapon pickup native: makes the prepared next value ($2C) the shown and collected value ($2A)
; In:  a6 = the pickup
; Out: $2A(a6) = $2C(a6); d0 = it
; Evidence: op $2CB2 at $13C3A, after Item_WeaponItemNextValue in the power-up's $60-tick coroutine $13C2E; code read
; ---------------------------------------------------------------------------
Item_WeaponItemShowNext:
	move.w	$2C(a6),d0
	move.w	d0,$2A(a6)
	rts
; ---------------------------------------------------------------------------
; Item_CollectWeapon - touch native of the weapon pickups: value 1-3 selects weapon type value-1 (same type: level +1 up to 6; new type: keeps the level, clears the charge); value 0 lowers the level by one
; In:  g_FoundBox = the toucher's box, $2A(a6) = the pickup's value
; Out: player record +$28 type, +$2A level, +$8 charge cleared on a switch
; Evidence: op $2CB2 at $13C44 after sound $47 (touch coroutine $13C40 of the spawn-action weapon pickups); +$28/+$2A/+$8 = weapon type/level/charge (NOTES Weapons); value 0 path inferred from the code: its spawner (spawn action 19, Item_SpawnWeaponRandomOrDownFromHit, table $13C7E = values 0-3) ran in the TAS (f80646, f80790), but a value-0 pickup being collected is not established
; ---------------------------------------------------------------------------
Item_CollectWeapon:
	bsr.w	Item_FoundBoxPlayer
	move.w	$28(a0),d0
	move.w	$2A(a0),d1
	move.w	$2A(a6),d2
	beq.s	loc_013D00
	subq.w	#$1,d2
	cmp.w	d0,d2
	beq.s	loc_013D06
	clr.w	$8(a0)
	move.w	d2,d0
	bra.s	loc_013D0E
loc_013D00:
	subq.w	#$1,d1
	bpl.s	loc_013D0E
	rts
loc_013D06:
	cmpi.w	#$6,d1
	beq.s	loc_013D16
	addq.w	#$1,d1
loc_013D0E:
	move.w	d0,$28(a0)
	move.w	d1,$2A(a0)
loc_013D16:
	rts
