; ---------------------------------------------------------------------------
; Boss_CatHead_SpawnRightPaw - spawner of the cat head's right paw (unlinked child $2C): Scr2_03B3C4 at (x $13C, y $55), H-flipped, on an x/y oscillator, then the shared paw body $03B428 (claw swipes with attack boxes EnemyAtk_HitKnockSpeed $20/$300)
; In:  d4/d5 = position (from Op2_SpawnChildOrWait)
; Out: jumps to Obj_SpawnBossUnlinked with a2 = Scr2_03B3C4
; Evidence: Op2_SpawnChildOrWait $2C dc.l $03B3B8 at $03B224; Op2_SetPos $13C/$55, Op2_ModSprAttr; TAS f87285; the claws at the right edge at f88800 (object at (321,81) in RAM f88800)
; ---------------------------------------------------------------------------
Boss_CatHead_SpawnRightPaw:
	lea.l	(Scr2_03B3C4).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
