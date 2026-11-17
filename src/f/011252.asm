; ---------------------------------------------------------------------------
; Player_AddStats - adds one player record to the per-frame aggregates when it is in play, otherwise clears its $36/$38/$3A fields
; In:  a0 = player record
; Out: g_PlayersInPlay +1 and the weapon level (+$2A), health/16 (+$4, negative as 0) and lives (+$6) folded into the max/min/sum globals
; Evidence: called for both records by Player_GatherStats every frame; +$2A is halved on a lost life ($1048E) and +$4/+$6 match the HUD ring and lives digit
; ---------------------------------------------------------------------------
Player_AddStats:
	btst	#$0,$25(a0)
	beq.s	loc_0112BA
	addq.w	#$1,(g_PlayersInPlay).w
	move.w	$2A(a0),d0
	add.w	d0,(g_WeaponLvlSum).w
	cmp.w	(g_WeaponLvlMax).w,d0
	ble.s	loc_011270
	move.w	d0,(g_WeaponLvlMax).w
loc_011270:
	cmp.w	(g_WeaponLvlMin).w,d0
	bge.s	loc_01127A
	move.w	d0,(g_WeaponLvlMin).w
loc_01127A:
	move.w	$4(a0),d0
	bpl.s	loc_011282
	moveq	#$0,d0
loc_011282:
	lsr.w	#$4,d0
	add.w	d0,(g_HealthSum).w
	cmp.w	(g_HealthMax).w,d0
	ble.s	loc_011292
	move.w	d0,(g_HealthMax).w
loc_011292:
	cmp.w	(g_HealthMin).w,d0
	bge.s	loc_01129C
	move.w	d0,(g_HealthMin).w
loc_01129C:
	move.w	$6(a0),d0
	add.w	d0,(g_LivesSum).w
	cmp.w	(g_LivesMax).w,d0
	ble.s	loc_0112AE
	move.w	d0,(g_LivesMax).w
loc_0112AE:
	cmp.w	(g_LivesMin).w,d0
	bge.s	loc_0112B8
	move.w	d0,(g_LivesMin).w
loc_0112B8:
	rts
loc_0112BA:
	clr.b	$3A(a0)
	clr.w	$38(a0)
	clr.w	$36(a0)
	rts
