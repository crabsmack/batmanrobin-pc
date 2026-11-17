; ---------------------------------------------------------------------------
; Enemy_BarrelThug_ThrowBarrel - barrel thug native with two inline offset words: spawns a barrel (Enemy_Barrel_Spawn) at its position + (dx, dy) with an x speed toward the targeted player ((player x - x) / 128 * 1.25; 0 with no target), handing it the thug's drop
; In:  a2 = inline dx, dy words, a3/a4/a6 = thug
; Out: d0 = 0 thrown (barrel's $18/$24(a3) speed; its $19/$1A = the thug's drop, the thug's $19 cleared), -1 when the pool is full
; Evidence: called from the throw animation at $025098 (dy -$26: overhead) and from the death path $024DAE after Op2_ClearPlayerTarget (dy -$1A: drops it); TAS f26895-f31863 (32); barrels Scr2_0253DC rolling at f28320
; ---------------------------------------------------------------------------
Enemy_BarrelThug_ThrowBarrel:
	move.w	$10(a4),d0
	ext.l	d0
	beq.s	loc_025388
	movea.w	d0,a0
	move.l	$2E(a0),d0
	sub.l	$3E(a6),d0
	asr.l	#$7,d0
	move.l	d0,d1
	asr.l	#$2,d1
	add.l	d1,d0
loc_025388:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	add.w	(a2)+,d4
	add.w	(a2)+,d5
	movem.l	a1-a6,-(a7)
	move.l	d0,-(a7)
	jsr	(Enemy_Barrel_Spawn).l
	bne.s	loc_0253C6
	move.l	(a7)+,d0
	move.l	d0,$24(a3)
	move.l	d0,$18(a3)
	movea.l	a6,a0
	movem.l	(a7)+,a1-a6
	move.b	$19(a6),$19(a0)
	move.w	$1A(a6),$1A(a0)
	clr.b	$19(a6)
	moveq	#$0,d0
	rts
loc_0253C6:
	move.l	(a7)+,d0
	movem.l	(a7)+,a1-a6
	moveq	#-$1,d0
	rts
