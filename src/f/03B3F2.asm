; ---------------------------------------------------------------------------
; Boss_CatHead_SpawnLeftPaw - spawner of the cat head's left paw (unlinked child $2A): Scr2_03B3FE at (x 4, y $55) on an x/y oscillator, paw body $03B428 (swipe animations $03B4AC/$03B54E with attack boxes)
; In:  d4/d5 = position (from Op2_SpawnChildOrWait)
; Out: jumps to Obj_SpawnBossUnlinked with a2 = Scr2_03B3FE
; Evidence: Op2_SpawnChildOrWait $2A dc.l $03B3F2 at $03B21C; Op2_SetPos 4/$55; TAS f87285; the purple paw at the left edge at f87560 and f88800 (object at (5,90) in RAM f88800)
; ---------------------------------------------------------------------------
Boss_CatHead_SpawnLeftPaw:
	lea.l	(Scr2_03B3FE).l,a2
	jmp	(Obj_SpawnBossUnlinked).l
