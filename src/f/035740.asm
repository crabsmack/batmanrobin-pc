; ---------------------------------------------------------------------------
; Enemy_PuffPlant_PuffHit - hit callback of the puff plant's rising puffs: EnemyAtk_HitTouch, and when it hit, the struck player's motion block gets a $A-frame stun and y speed -4 px/frame (blown upward)
; In:  a2 = the struck box (+$2E owner a6), the touching puff's callback arguments
; Out: Z/flags from EnemyAtk_HitTouch (nonzero = no hit)
; Evidence: callback long $00035740 in the puff a4 script $356EE at $3570A (inside the Scr2_03554C span; spawned by the plant body's FxSpawnOff $355FC); `move.b #$A,1(a1)` = a3 stun count, `move.w #$FFFC,$1C(a1)` = y velocity high word; TAS f96393-f96411 (4 hits)
; ---------------------------------------------------------------------------
Enemy_PuffPlant_PuffHit:
	jsr	(EnemyAtk_HitTouch).l
	bne.s	loc_03575C
	movea.w	$2E(a2),a1
	movea.w	$8(a1),a1
	move.b	#$A,$1(a1)
	move.w	#$FFFC,$1C(a1)
loc_03575C:
	rts
