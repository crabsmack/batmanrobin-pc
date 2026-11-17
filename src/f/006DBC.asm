; ---------------------------------------------------------------------------
; Op5_SetQuota - a5 op: sets the wave's spawn quota $E (one player) / $10 (two players) and the script address $16 it jumps to once that many enemies spawned; clears the spawn count $C
; In:  a5 = script: word quota 1P, word quota 2P, word offset to the after-quota script
; Out: NEXT
; Evidence: $E/$10/$16, clr.w $C; the spawn ops' tail $7052 compares $C with the quota and sets $1E = $16; script uses $21DAE (`6DBC 01F4 01F4 004A`), $21DEC; 4 corpus runs
; ---------------------------------------------------------------------------
Op5_SetQuota:
	move.w	(a5)+,$E(a6)
	move.w	(a5)+,$10(a6)
	movea.l	a5,a0
	adda.w	(a5)+,a0
	move.l	a0,$16(a6)
	clr.w	$C(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_SetPopulationCap - a5 op: sets the population caps (one player $12, two players $14): the wave only counts down and spawns while g_ObjPopulation is below the cap
; In:  a5 = script: word cap 1P, word cap 2P
; Out: NEXT
; Evidence: move.w (a5)+,$12 / $14; read by VM5_RunObject; 74 script uses, e.g. $1693C (`6DD4 0003 0004`); 29 corpus runs
; ---------------------------------------------------------------------------
Op5_SetPopulationCap:
	move.w	(a5)+,$12(a6)
	move.w	(a5)+,$14(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_ClearOrigin - a5 op: clears the wave's spawn origin offset $26/$28 (added to every spawn entry's x/y)
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: clr.w $26 / clr.w $28; read by the spawn routine $6FEA; script uses $21DC4, $21DD4; 37 corpus runs
; ---------------------------------------------------------------------------
Op5_ClearOrigin:
	clr.w	$26(a6)
	clr.w	$28(a6)
	movea.w	(a5)+,a0
	jmp	(a0)
