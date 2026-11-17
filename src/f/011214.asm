; ---------------------------------------------------------------------------
; Player_GatherStats - recomputes the per-frame aggregates over the players in play: count, and max/min/sum of weapon level, health/16 and lives
; In:  g_Player1, g_Player2
; Out: g_PlayersInPlay and g_WeaponLvlMax..g_LivesSum (mins start at $7FFF)
; Evidence: first call of HUD_Update, every frame from frame 532; g_PlayersInPlay is read by the a2 op $39AC and g_WeaponLvlMax by $3CEE
; ---------------------------------------------------------------------------
Player_GatherStats:
	moveq	#$0,d0
	move.w	#$7FFF,d1
	move.w	d0,(g_PlayersInPlay).w
	move.w	d0,(g_WeaponLvlMax).w
	move.w	d1,(g_WeaponLvlMin).w
	move.w	d0,(g_WeaponLvlSum).w
	move.w	d0,(g_HealthMax).w
	move.w	d1,(g_HealthMin).w
	move.w	d0,(g_HealthSum).w
	move.w	d0,(g_LivesMax).w
	move.w	d1,(g_LivesMin).w
	move.w	d0,(g_LivesSum).w
	lea.l	(g_Player1).l,a0
	bsr.w	Player_AddStats
	lea.l	(g_Player2).l,a0
