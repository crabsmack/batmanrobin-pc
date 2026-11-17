; ---------------------------------------------------------------------------
; Stage1_LaunchBalloonA - launcher native: spawns a balloon bomb (Scr2_0203E0) at the launcher's position and gives it the next pair of bob heights from the launcher's table, wrapping to Tbl_020354 at the end marker
; In:  a6 = launcher; $1E(a6) = cursor into a table of (word height1, word height2) pairs ended by a negative word
; Out: d0 = 0 and the balloon's $2A/$2C = the two screen-y heights, cursor advanced; d0 != 0 when the pool is full
; Evidence: a2 op $2CB2 (relative native call) from Scr2_020094 and Scr2_01FFB8 (e.g. $0200B4); TAS f13918-f18166 (84 frames); RAM f14400: balloons with $2A/$2C = $32/$AF, $4B/$AF, $64/$C8
; ---------------------------------------------------------------------------
Stage1_LaunchBalloonA:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	movem.l	a2-a6,-(a7)
	lea.l	(Scr2_0203E0).l,a2
	jsr	(Obj_SpawnEnemy).l
	movea.l	a6,a1
	movem.l	(a7)+,a2-a6
	bne.s	loc_0202D6
	movea.l	$1E(a6),a0
	move.w	(a0)+,$2A(a1)
	move.w	(a0)+,$2C(a1)
	tst.w	(a0)
	bpl.s	loc_0202D0
	lea.l	(Tbl_020354).l,a0
loc_0202D0:
	move.l	a0,$1E(a6)
	moveq	#$0,d0
loc_0202D6:
	rts
; ---------------------------------------------------------------------------
; Stage1_LaunchBalloonB - launcher native: the same balloon-bomb spawn as Stage1_LaunchBalloonA but wrapping its height cursor to Tbl_02036E
; In:  a6 = launcher, $1E(a6) = height-pair cursor
; Out: d0 = 0 and the balloon's $2A/$2C set, cursor advanced; d0 != 0 when the pool is full
; Evidence: op $2CB2 from Scr2_01FFB8 ($01FFE2...) and Scr2_020094 ($0200F6...); executed in the TAS and fuzz-14400..18000. A never-run copy at $020316 (raw) wraps to $020388
; ---------------------------------------------------------------------------
Stage1_LaunchBalloonB:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	movem.l	a2-a6,-(a7)
	lea.l	(Scr2_0203E0).l,a2
	jsr	(Obj_SpawnEnemy).l
	movea.l	a6,a1
	movem.l	(a7)+,a2-a6
	bne.s	loc_020314
	movea.l	$1E(a6),a0
	move.w	(a0)+,$2A(a1)
	move.w	(a0)+,$2C(a1)
	tst.w	(a0)
	bpl.s	loc_02030E
	lea.l	(Tbl_02036E).l,a0
loc_02030E:
	move.l	a0,$1E(a6)
	moveq	#$0,d0
loc_020314:
	rts
