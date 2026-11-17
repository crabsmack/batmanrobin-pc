; ---------------------------------------------------------------------------
; ToyChain_MakeHanging8 - chain-child spawner for Op2_SpawnChild: ToyChain_SpawnInline with 8 links, parameter table $03B174 (link energy $60, touch callback EnemyAtk_HitKnockSpeed $10/$200) and script $03AEF8 (random speed, sway oscillator, charge cycle $038810, coroutine 0 $03822C, chain body $03802E)
; In:  d4/d5 = position (from Op2_SpawnChild)
; Out: d0 = 0 / a0 = chain block, or nonzero when the pool is full
; Evidence: jsr ToyChain_SpawnInline(pc) / dc.w 8 / dc.l $03B174 / dc.l $03AEF8; dc.l $03AEEA in Scr2_03A9B4+8 and Scr2_03AA26+8; TAS f96963-f97271; RAM f97380: the chain object (owner $B) beside its propeller toy
; ---------------------------------------------------------------------------
ToyChain_MakeHanging8:
	jsr	ToyChain_SpawnInline(pc)
