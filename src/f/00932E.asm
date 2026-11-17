; ---------------------------------------------------------------------------
; Wave_Start - spawns a wave-director object running a5 script a5 (handler $6A14) and counts it in g_WaveCount
; In:  a5 = a5-VM script, d4/d5 = position
; Out: d0 = 0, a6 = the object; Z clear when the pool is short
; Evidence: $8A08 one block; $A = ++g_WaveSerial, $1E script, $22/$24 = d4/d5, $E-$14 = $7FFF (no population cap), $16 = $939E (a lone `$6A72` end op); $6A14 runs the script while g_ObjPopulation is below cap $12 (one player) or $14 (two)
; ---------------------------------------------------------------------------
Wave_Start:
	lea.l	(VM5_RunObject).l,a0
	bsr.w	Obj_New
	bne.s	loc_00939C
	addq.w	#$1,(g_WaveSerial).w
	move.w	(g_WaveSerial).w,$A(a6)
	move.l	a5,$1E(a6)
	move.w	d4,$22(a6)
	move.w	d5,$24(a6)
	clr.w	$26(a6)
	clr.w	$28(a6)
	move.w	#$7FFF,$E(a6)
	move.w	#$7FFF,$10(a6)
	move.w	#$7FFF,$12(a6)
	move.w	#$7FFF,$14(a6)
	move.l	#$939E,$16(a6)
	clr.w	$32(a6)
	clr.w	$8(a6)
	clr.w	$C(a6)
	clr.l	$2A(a6)
	clr.l	$2E(a6)
	move.w	a6,d0
	addi.w	#$36,d0
	move.w	d0,$34(a6)
	addq.w	#$1,(g_WaveCount).w
	moveq	#$0,d0
loc_00939C:
	rts
