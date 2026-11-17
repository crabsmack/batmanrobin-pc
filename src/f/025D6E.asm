; ---------------------------------------------------------------------------
; Boss_Stage2_RampExplosions - native fragment of the defeated boss's coroutine 0 (while no enemy is left and the end timer runs): raises the explosion rate in script var 3 by $80 per frame (saturating at $FFFF), then falls into Boss_Stage2_TickExplosions
; In:  a2 script context, a4 = boss script block (+$28 var 3)
; Out: falls through
; Evidence: addi.w #$80,$28(a4) / bcc; var 3 starts at $800 (Op2_SetVar3 at $025D54 after BossMeter_Defeated); reached after op $107C (timer 2 tick, $2D00 = 720 frames at speed $100); TAS f33964-f34682 (719 frames)
; ---------------------------------------------------------------------------
Boss_Stage2_RampExplosions:
	addi.w	#$80,$28(a4)
	bcc.s	Boss_Stage2_TickExplosions
	move.w	#$FFFF,$28(a4)
	bra.s	Boss_Stage2_TickExplosions
