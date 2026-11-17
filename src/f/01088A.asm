; ---------------------------------------------------------------------------
; Hitbox_SpawnAtContact - spawns the a4 effect script a1 at the centre of the overlap between box a2 and the rectangle d4-d7 (via $91F2 on g_SpawnCtx), preserving the caller's registers
; In:  a1 = a4 script (e.g. hit sparks Scr4_010964, Scr4_0108CE, Scr4_0108F4), a2 = struck box, d4/d6 = x1/x2, d5/d7 = y1/y2 of the other box, a3 = velocity block passed on
; Out: none (d3-d7/a0/a2 preserved)
; Evidence: callers the hit callbacks $106C0, $106E2, $107B0, PlayerAtk_ApplyHit, $41F0E; the spawned scripts run 3-frame animations (frame words $2350/$235A/$2364 in Scr4_0108F4); 391 TAS frames
; ---------------------------------------------------------------------------
Hitbox_SpawnAtContact:
	movem.l	d3-d7/a0/a2,-(a7)
	movea.l	a1,a0
	bsr.w	Hitbox_OverlapCenter
	moveq	#$0,d3
	moveq	#$0,d2
	swap	d4
	swap	d5
	jsr	Fx_RunScript(pc)
	movem.l	(a7)+,d3-d7/a0/a2
	rts
