; ---------------------------------------------------------------------------
; Weapon_KeepShotArtOne - for one player, loads the art of the shot frame for its weapon type/level (Tbl_010A1E, flying set when g_PlayMode bit 1) and releases the previously held frame 20 frames after a change, so shots spawn without an art load
; In:  a6 = player record (+$25 bit 0 in play, +$28 type, +$2A level), a5 = its 3-word slot in g_ShotArtSlots
; Out: slot +0 = frame held, +2 = release countdown, +4 = old frame; Spr_ChangeFrame/Spr_ReleaseFrame called
; Evidence: TAS RAM: slot frame $238C (type 0) on foot, $2594 with type 2 level 6, $2684 in the flying section (g_PlayMode = 2, frames 36000-79200); Tbl_010A1E = 6 rows x 8 levels of frame words; nothing is drawn (Spr_ChangeFrame only)
; ---------------------------------------------------------------------------
Weapon_KeepShotArtOne:
	moveq	#$0,d1
	btst	#$0,$25(a6)
	beq.s	loc_0109C8
	move.w	$28(a6),d1
	lsl.w	#$3,d1
	add.w	$2A(a6),d1
	add.w	d1,d1
	move.w	(g_PlayMode).w,d0
	andi.w	#$2,d0
	beq.s	loc_0109C4
	addi.w	#$30,d1
loc_0109C4:
	move.w	Tbl_010A1E(pc,d1.w),d1
loc_0109C8:
	cmp.w	(a5),d1
	beq.s	loc_010A08
	moveq	#$0,d2
	jsr	(Spr_ChangeFrame).l
	move.w	(a5),d2
	cmp.w	d1,d2
	beq.s	loc_010A08
	move.w	d1,(a5)
	tst.w	d2
	beq.s	loc_010A08
	tst.w	$2(a5)
	beq.s	loc_0109FC
	move.w	#$14,$2(a5)
	move.w	d2,d0
	move.w	$4(a5),d2
	move.w	d0,$4(a5)
	jmp	(Spr_ReleaseFrame).l
loc_0109FC:
	move.w	#$14,$2(a5)
	move.w	d2,$4(a5)
loc_010A06:
	rts
loc_010A08:
	tst.w	$2(a5)
	beq.s	loc_010A06
	subq.w	#$1,$2(a5)
	bne.s	loc_010A06
	move.w	$4(a5),d2
	jmp	(Spr_ReleaseFrame).l
