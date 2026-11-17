; ---------------------------------------------------------------------------
; Player_InitRecord - resets a player record for a new game or a join: score 0, health $100, charge 0, countdown 0, and the starting lives, weapon type and weapon level
; In:  a0 = player record (g_Player1/g_Player2)
; Out: +0 = 0, +4 = $100, +8 = 0, +$26 = 0, +6 = g_StartLives, +$28 = g_StartWeaponType, +$2A = g_StartWeaponLvl
; Evidence: callers $11050 (game start, bsr and bra) and Player_JoinOrSpawn (a credit join); TAS frame 477; `clr.l 0(a0)` is a `dc.w` insn line
; ---------------------------------------------------------------------------
Player_InitRecord:
	move.w	#$100,$4(a0)
	dc.w	$42A8,$0000	; insn: clr.l $0(a0)
	clr.w	$8(a0)
	clr.w	$26(a0)
	move.w	(g_StartLives).w,$6(a0)
	move.w	(g_StartWeaponType).w,$28(a0)
	move.w	(g_StartWeaponLvl).w,$2A(a0)
	rts
