; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdRandom - command 7: picks one of byte 1 entries of the word-offset table after the command (Rand_Next's high word x byte 1) and continues at the entry + its offset
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1; a4 = the boss's VM block
; Out: d0 = Rand_Next's, d1 = 2 x the pick (upper word: the product's low word), a0 = the picked target; then Boss_Hatter_RoadCmdNext
; Evidence: jsr Rand_Next / ext.w d1 / mulu.w d0,d1 / swap d1 / add.w d1,d1 / adda.w d1,a0 / adda.w (a0),a0 / bra.s Boss_Hatter_RoadCmdNext; reached by Boss_Hatter_RoadCmdTbl entry 7 ($03DD40); the road schedules the Hatter's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0 from $03E11A-$03F25A) use it; the corpus ran commands 0, 2 and 8 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdRandom:
	jsr	(Rand_Next).l
	ext.w	d1
	mulu.w	d0,d1
	swap	d1
	add.w	d1,d1
	adda.w	d1,a0
	adda.w	(a0),a0
	bra.s	Boss_Hatter_RoadCmdNext
