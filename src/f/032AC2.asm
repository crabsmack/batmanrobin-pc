; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnDomeRight - spawns a right-facing missile dome of the dome cannon's third form: spawn action $2D, dome body $2FC6C (palette 1 without the H flip); the script that follows ($32ADA-$32D4D) holds the domes' launch patterns: fans of Enemy_Missile_SpawnSlow to the left ($250/$200/$1B0) or right ($FF80/0/$50) and Enemy_Missile_SpawnFast
; In:  d4/d5 = position (the parent's)
; Out: Obj_SpawnEnemyUnlinked's result
; Evidence: dc.l of Op2_SpawnChildOrWait (fields $30/$26) in Scr2_0320EA only; 30 GosubArgs ops to $316CE and one to $31710 in Scr2_032ACE; TAS f75008
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnDomeRight:
	lea.l	(Scr2_032ACE).l,a2
	jmp	(Obj_SpawnEnemyUnlinked).l
