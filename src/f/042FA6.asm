; ---------------------------------------------------------------------------
; Enemy_VentTank_PuffHit - hit callback of the gas puffs the vent tank blows up from its nozzles: EnemyAtk_HitTouchSpark, then the struck player's motion block gets a 2-frame stun and y speed -4 px/frame (blown upward)
; In:  a2 = the struck box (+$2E owner a6), a6 = the puff
; Out: a1 = the struck owner's a3; d0 as EnemyAtk_HitTouchSpark
; Evidence: callback long $00042FA6 at $42F70 in the a4 puff script $42F4E (Op4_HitPlayersOnPhase, damage $10, anim = the puff frames $497A-$49CA at $42FC2), forked by the vent scripts $45B8C/$45BA8/$45BC4/$45BE0 that the vent tank runs with Fx_SpawnAtOfs (0, -$18); also forked at $4718A/$471A8 (not reached); `move.b #2,1(a1)` stun count, `move.w #-4,$1C(a1)` y velocity; TAS 121172-121506 (7 hits)
; ---------------------------------------------------------------------------
Enemy_VentTank_PuffHit:
	jsr	(EnemyAtk_HitTouchSpark).l
	movea.w	$2E(a2),a1
	movea.w	$8(a1),a1
	move.b	#$2,$1(a1)
	move.w	#$FFFC,$1C(a1)
	rts
