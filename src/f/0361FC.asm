; ---------------------------------------------------------------------------
; Enemy_BouncingBomb_TakeKnockback - native when a player strikes a bouncing bomb: adds the struck box's knockback push (+$22) to the bomb's x velocity and target speed, batting it away
; In:  a3 = the bomb's motion block; g_StruckBox
; Out: $18/$24(a3) changed; re-enters the a2 VM
; Evidence: a2 native at $361FA in Scr2_0361A8 after Op2_SwitchAttacker (players 1/2); TAS f86759 (once)
; ---------------------------------------------------------------------------
Enemy_BouncingBomb_TakeKnockback:
	movea.l	(g_StruckBox).w,a0
	move.l	$22(a0),d0
	add.l	d0,$18(a3)
	add.l	d0,$24(a3)
	jsr	($7FE).w
