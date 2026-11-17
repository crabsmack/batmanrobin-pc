; ---------------------------------------------------------------------------
; Enemy_BombDropper_DropBomb - native in the bomb dropper's animation: spawns a bouncing bomb at its position plus the inline offset, aimed at the targeted player (x speed = (player x - own x) / 128 * 1.25 into the bomb's velocity and target speed, same target record)
; In:  a6/a4/a3 = the dropper; a2 = script: words dx, dy
; Out: d0 = 0, or -1 when the pool is full
; Evidence: `2CB2 0050 0000 0014` at $360F4 in the drop animation (frames $FC40-$FC72); TAS f79427-f86692 (25 drops); f079440 a bomb falling below the dropper
; ---------------------------------------------------------------------------
Enemy_BombDropper_DropBomb:
	move.w	$10(a4),d0
	movea.w	d0,a0
	beq.s	loc_03615E
	move.l	$2E(a0),d0
	sub.l	$3E(a6),d0
	asr.l	#$7,d0
	move.l	d0,d1
	asr.l	#$2,d1
	add.l	d1,d0
loc_03615E:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	add.w	(a2)+,d4
	add.w	(a2)+,d5
	movem.l	a1-a6,-(a7)
	movem.l	d0/a0,-(a7)
	jsr	(Enemy_BouncingBomb_Spawn).l
	bne.s	loc_036192
	movem.l	(a7)+,d0/a0
	move.w	a0,$10(a4)
	move.l	d0,$24(a3)
	move.l	d0,$18(a3)
	movem.l	(a7)+,a1-a6
	moveq	#$0,d0
	rts
loc_036192:
	addq.l	#$8,a7
	movem.l	(a7)+,a1-a6
	moveq	#-$1,d0
	rts
