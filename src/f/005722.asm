; ---------------------------------------------------------------------------
; Op4_JitterSpawnHeading - a4 op: adds a random amount in [-w/2, w/2) heading units ($400 per turn) to the spawn heading register d3
; In:  a4 = script: word w; d3 = spawn heading (16.16)
; Out: NEXT; d3 jittered
; Evidence: Rand_Next * w added to d3; script uses $1A45A, $1A54E (`5722 0400`: any direction, then Op4_SpawnAddVel), $14BCE; 66 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterSpawnHeading:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,d3
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_JitterDrawXY - a4 op: shakes the drawn position: adds random [-wx/2, wx/2) / [-wy/2, wy/2) px to the drawn x/y words in d4/d5 (low words after the motion op's swap)
; In:  a4 = script: word wx, word wy; d4/d5 = drawn x/y (pixel in the low word)
; Out: NEXT; d4/d5 shaken
; Evidence: Rand_Next * w then swap before add.l: the product's high word lands in the low (pixel) word; uses $1A5C6, $1F2BC between the motion op $6286 and Op4_SetResume + the draw op $67D2; 1 corpus run (the TAS)
; ---------------------------------------------------------------------------
Op4_JitterDrawXY:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	swap	d0
	add.l	d0,d4
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	swap	d0
	add.l	d0,d5
	movea.w	(a4)+,a0
	jmp	(a0)
