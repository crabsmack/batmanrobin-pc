; ---------------------------------------------------------------------------
; Rand_Next - steps the game's pseudo-random generator (shift/xor over a word and a long of state) and returns the new random word
; In:  $F5FC word and $F5FE long = state (seeded $79/$4B/$D3 by Main_Start)
; Out: d0 = random word sign-extended to a long; d1 preserved
; Evidence: rol/eor/swap mixing of the two state words, written back each call; Frame_End calls it once per frame and 70+ sites ($9C4, $2FCE, $56E6 ...) call it for random choices; every TAS frame from f18
; ---------------------------------------------------------------------------
Rand_Next:
	move.l	d1,-(a7)
	move.w	(g_RandState).w,d0
	move.l	(g_RandState2).w,d1
	rol.l	#$1,d1
	eor.w	d0,d1
	swap	d1
	eor.w	d1,d0
	rol.w	#$1,d0
	eor.w	d0,d1
	swap	d1
	eor.w	d1,d0
	move.l	d1,(g_RandState2).w
	move.w	d0,(g_RandState).w
	move.l	(a7)+,d1
	ext.l	d0
	rts
