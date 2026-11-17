; ---------------------------------------------------------------------------
; Enemy_JetTrooper_ShotHit - hit callback of the jetpack trooper's gun shots: EnemyAtk_HitTouch; on a hit it only loads the struck player's motion block into a1 (no extra effect, unlike Enemy_PuffPlant_PuffHit)
; In:  a2 = the struck box (+$2E owner a6), a6 = the shot
; Out: Z/flags from EnemyAtk_HitTouch (nonzero = no hit), a1 = the struck owner's a3 when hit
; Evidence: callback long $000429BC in the a4 shot script $42964 (Op4_HitPlayersOnPhase at $42986), fired by Op2_FxSpawnRegs at $42532 in the trooper body ($423CE, Scr2_04204C span); TAS 125825-125837 (5 hits, stage 4 tube section), scenario-level-skip 13475-13774
; ---------------------------------------------------------------------------
Enemy_JetTrooper_ShotHit:
	jsr	(EnemyAtk_HitTouch).l
	bne.s	loc_0429CC
	movea.w	$2E(a2),a1
	movea.w	$8(a1),a1
loc_0429CC:
	rts
