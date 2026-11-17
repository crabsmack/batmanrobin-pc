; ---------------------------------------------------------------------------
; Enemy_BoostImpulseX - a2 native right after Op2_TakeHit in the card-hall enemy body $035AA6: adds a quarter of the motion block's x impulse to itself (knockback x1.25), then continues the script
; In:  a3 = the struck enemy's motion block (+2 = x impulse, 16.16)
; Out: $2(a3) *= 1.25; re-enters the a2 VM ($7FE)
; Evidence: native word at $35AE6 in the body $035AA6 (attack box EnemyAtk_HitKnockSpeed, energy $100), spawned by $03699C (Scr2_0369A8) and the raw spawners $036934/$036968; TAS f111035-f112744 (5 hits, the enemy off screen in every RAM sample, so its look is not identified)
; ---------------------------------------------------------------------------
Enemy_BoostImpulseX:
	move.l	$2(a3),d0
	asr.l	#$2,d0
	add.l	d0,$2(a3)
	jsr	($7FE).w
