; ---------------------------------------------------------------------------
; Op4_SetDamage - a4 op: sets the damage word $12(a6) that the object's hit natives deal (Shot_DamageTarget adds it to the struck box +$20 and subtracts it from its energy)
; In:  a4 = script: word damage
; Out: NEXT
; Evidence: move.w (a4)+,$12(a6); Shot_DamageTarget reads $12(a6); script uses $1AB6C ($10), $26B86, $26CF4; 75 corpus runs
; ---------------------------------------------------------------------------
Op4_SetDamage:
	move.w	(a4)+,$12(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildSetDamage - a4 op: sets the damage word $12 of the last spawned child (a5)
; In:  a4 = script: word damage; a5 = child
; Out: NEXT
; Evidence: move.w (a4)+,$12(a5); 159 a4-script uses, e.g. $25B7E (`56EE 0010` then a yield), $1F0E4, the shot scripts; 151 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildSetDamage:
	move.w	(a4)+,$12(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_JitterSpawnX - a4 op: adds a random offset in [-w/2, w/2) px to the spawn x register d4 (16.16) for the next spawn op
; In:  a4 = script: word w; d4 = spawn x (16.16)
; Out: NEXT; d4 jittered
; Evidence: Rand_Next (signed word) * w added to d4, i.e. +-w/2 in the integer word; script uses $1A4A4 (`56F6 0037 5606`), $1AC96; 8 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterSpawnX:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,d4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_JitterSpawnXY - a4 op: adds random offsets in [-wx/2, wx/2) and [-wy/2, wy/2) px to the spawn position registers d4/d5
; In:  a4 = script: word wx, word wy; d4/d5 = spawn x/y (16.16)
; Out: NEXT; d4/d5 jittered
; Evidence: two Rand_Next * w adds to d4 and d5; script uses $14A18 (`5702 0008 0008`), $1A404, $1A454 before spawn ops; 63 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterSpawnXY:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,d4
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,d5
	movea.w	(a4)+,a0
	jmp	(a0)
