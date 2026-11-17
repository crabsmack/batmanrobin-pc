; ---------------------------------------------------------------------------
; Enemy_Barrel_KnockByHit - a2 native fragment of the barrel's script: when a struck barrel is hit by the attacker kind of its switch, adds half the hit's knockback (box +$22) to its x velocity and x target speed, then resumes the script
; In:  a3 = the barrel's motion block; g_StruckBox
; Out: $18(a3) / $24(a3) += knockback / 2; d0 = it; a0 = the box; the script resumes at Scr2_025466 (jsr VM2_Enter)
; Evidence: Op2_Native at $02544E in Scr2_0253DC (Enemy_Barrel_Spawn's barrel), after Op2_SwitchAttacker; movea.l g_StruckBox,a0 / move.l $22(a0),d0 / asr.l #1,d0 / add.l d0,$18(a3) / add.l d0,$24(a3) / jsr VM2_Enter; raw until crane-crash (2026-10-30): no corpus run reaches it
; ---------------------------------------------------------------------------
Enemy_Barrel_KnockByHit:
	movea.l	(g_StruckBox).w,a0
	move.l	$22(a0),d0
	asr.l	#$1,d0
	add.l	d0,$18(a3)
	add.l	d0,$24(a3)
	jsr	($7FE).w
