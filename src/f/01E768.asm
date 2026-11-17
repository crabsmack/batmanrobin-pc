; ---------------------------------------------------------------------------
; Boss_Claw_HitCallback - hit callback of the claw's attack box: EnemyAtk_HitKnock, and when it hit, holds the claw for 8 frames ($30(a6), counted by Boss_Claw_TickHitPause)
; In:  as EnemyAtk_HitKnock (a6 = the claw)
; Out: Z/d0 from EnemyAtk_HitKnock
; Evidence: callback long $0001E768 with args $30/$380 in the Op2_SetAttackBox spec at $1E77C (Scr2_01E778); TAS f12387-f12548 (11 frames)
; ---------------------------------------------------------------------------
Boss_Claw_HitCallback:
	jsr	(EnemyAtk_HitKnock).l
	bne.s	loc_01E776
	move.w	#$8,$30(a6)
loc_01E776:
	rts
