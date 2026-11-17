; ---------------------------------------------------------------------------
; Boss_Stage2_TickExplosions - native fragment of the defeated boss's coroutine 0: adds the rate (var 3) to the accumulator var 2; on a carry resumes at $025D94 (Boss_Stage2_ExplosionDue), else yields ($025D8E Op2_Return)
; In:  a2 script context, a4 = boss script block (+$26 var 2, +$28 var 3)
; Out: jsr $7FE.w
; Evidence: add.w d0,$26(a4) / bcs; runs alone (no ramp) while enemies remain (Op2_BranchPopulationNz at $025D64); TAS f33605-f34682 (1078 frames; defeat at f33605)
; ---------------------------------------------------------------------------
Boss_Stage2_TickExplosions:
	move.w	$28(a4),d0
	add.w	d0,$26(a4)
	bcs.s	Boss_Stage2_ExplosionDue
	jsr	($7FE).w
