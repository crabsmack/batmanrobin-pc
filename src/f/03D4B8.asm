; ---------------------------------------------------------------------------
; Boss_Hatter_DropHopRabbit - a2 native (op $2CBC): spawns a hopping bomb rabbit (Enemy_BombRabbit_SpawnHop) at the hat's x, y + 10
; In:  a6 = the boss
; Out: a2-a6 preserved
; Evidence: dc.l $0003D4B8 in the attack script Scr2_03E39A ($03E498, $03E4A4, $03E51A) and the never-run Scr2_03E532; TAS f114032-f114152 (2 frames)
; ---------------------------------------------------------------------------
Boss_Hatter_DropHopRabbit:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	addi.w	#$A,d5
	movem.l	a2-a6,-(a7)
	jsr	Enemy_BombRabbit_SpawnHop(pc)
	movem.l	(a7)+,a2-a6
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_DropRabbit - a2 native (op $2CBC): spawns a bomb rabbit that drops in (Enemy_BombRabbit_SpawnDropIn) at the hat's x, y + 70 (below the hat)
; In:  a6 = the boss
; Out: a2-a6 preserved
; Evidence: dc.l $0003D4D2 at $03E064/$03E070 in the boss's coroutine-3 script Scr2_03E034 (every $3C0 ticks); ctarget in fuzz-115200 and the TAS
; ---------------------------------------------------------------------------
Boss_Hatter_DropRabbit:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	addi.w	#$46,d5
	movem.l	a2-a6,-(a7)
	jsr	Enemy_BombRabbit_SpawnDropIn(pc)
	movem.l	(a7)+,a2-a6
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_LaunchFx - a2 native (op $2CBC + word offset): runs the a4 spawn script at the inline offset from 10 px right of the hat's position (d2/d3 = 0); used to throw spinning toy figures in arcs (scripts $03F494/$03F548: Op4_Spawn of the thrown toy + Arc_PlanToArgs)
; In:  a2 = the offset word, a6 = the boss
; Out: a2 past the word; as Fx_RunScript
; Evidence: dc.l $0003D4EC in Scr2_03E39A ($03E47C, $03E48A, $03E4FE ...) and Scr2_03E532; ctarget in 4 corpus runs (fuzz-113400, level-skip)
; ---------------------------------------------------------------------------
Boss_Hatter_LaunchFx:
	moveq	#$0,d2
	moveq	#$0,d3
	move.l	$3E(a6),d4
	move.l	$42(a6),d5
	addi.l	#$A0000,d4
	movea.l	a2,a0
	adda.w	(a2)+,a0
	jmp	(Fx_RunScript).l
