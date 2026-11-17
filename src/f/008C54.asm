; ---------------------------------------------------------------------------
; Hitbox_LinkEnemyList - appends collision box a0 to the end of g_EnemyBoxList (after g_EnemyBoxLast, before g_EnemyBoxTail)
; In:  a0 = box (pool block)
; Out: a0 +0 next = g_EnemyBoxTail, +2 prev = the old last box; g_EnemyBoxLast = a0; a1 preserved (usp)
; Evidence: callers Hitbox_LinkByKind_Enemy (kinds 8/$C), Op4_BodyBox $66E2 (shootable effect boxes), $374C2; Hitbox_FindEnemyBox scans the list; TAS 2147 frames
; ---------------------------------------------------------------------------
Hitbox_LinkEnemyList:
	move.l	a1,usp
	movea.w	(g_EnemyBoxLast).w,a1
	move.w	a0,(g_EnemyBoxLast).w
	move.w	a0,(a1)
	move.w	a1,$2(a0)
	move.w	#$DEB8,(a0)
	move.l	usp,a1
	rts
