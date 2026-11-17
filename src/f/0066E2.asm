; ---------------------------------------------------------------------------
; Op4_BodyBox - a4 op: keeps a shootable box on the object: creates it on first use (enemy box list, id, energy, no hit callback) and recentres it on d4/d5 every run; branches once the box's energy is used up
; In:  a4 = script: word branch offset (energy <= 0), word box id, word energy, word half-width, word half-height; d4/d5 = drawn x/y words
; Out: NEXT or the branch; $14(a6) = the box
; Evidence: ObjPool_Alloc + $8C54 (enemy box list), +$30 = $82C (rts), +$10 id, +$1E energy, rect +$12-$18 = d4/d5 -+ w/h; tst.w $1E(a0) / ble Op4_Goto; script uses $1B558 (`66E2 001E 000C 0010 000A 000A`), $359E0; 23 corpus runs
; ---------------------------------------------------------------------------
Op4_BodyBox:
	move.w	$14(a6),d0
	beq.s	loc_006712
	movea.w	d0,a0
	tst.w	$1E(a0)
	ble.w	Op4_Goto
	addq.l	#$6,a4
loc_0066F4:
	move.w	(a4)+,d0
	move.w	(a4)+,d1
	move.w	d0,d2
	move.w	d1,d3
	neg.w	d0
	neg.w	d1
	add.w	d4,d0
	add.w	d5,d1
	add.w	d4,d2
	add.w	d5,d3
	movem.w	d0-d3,$12(a0)
	movea.w	(a4)+,a0
	jmp	(a0)
loc_006712:
	bsr.w	ObjPool_Alloc
	bne.s	loc_00673C
	bsr.w	Hitbox_LinkEnemyList
	move.l	#$82C,$30(a0)
	clr.w	$3C(a0)
	clr.w	$20(a0)
	move.w	a0,$14(a6)
	addq.w	#$2,a4
	move.w	(a4)+,$10(a0)
	move.w	(a4)+,$1E(a0)
	bra.s	loc_0066F4
loc_00673C:
	adda.w	#$A,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SwitchHeading - a4 op: branches to one of n offsets by the object's heading: index = ((heading + rotation) & $3FE) * n / $400
; In:  a4 = script: word rotation, word n, n word offsets (each relative to itself)
; Out: the chosen branch
; Evidence: andi #$3FE / mulu n / lsr 10 / table index; script uses $14754 (`6744 0140 0008` + 8 offsets), $28C46; 21 corpus runs
; ---------------------------------------------------------------------------
Op4_SwitchHeading:
	move.w	$2A(a6),d0
	add.w	(a4)+,d0
	andi.w	#$3FE,d0
	mulu.w	(a4)+,d0
	lsr.l	#$8,d0
	lsr.w	#$2,d0
	add.w	d0,d0
	adda.w	d0,a4
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_BranchScreenClear - a4 op: branches while a screen-clear pause runs (g_ScreenClearTimer nonzero); otherwise continues past the offset
; In:  a4 = script: word branch offset
; Out: NEXT or the branch
; Evidence: tst.w g_ScreenClearTimer / bne Op4_Goto; script uses $1B540 (`675E 0036`), $359D0, $42D14; 17 corpus runs
; ---------------------------------------------------------------------------
Op4_BranchScreenClear:
	tst.w	(g_ScreenClearTimer).w
	bne.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
