; ---------------------------------------------------------------------------
; Stage1_StartFireFlicker - bombing native: starts a run-list object that flickers palette line 1 between two 16-colour palettes at random intervals (the fire's glow, inferred)
; In:  -
; Out: d0 = 0 ($18/$1A cleared); d0 = -1 when the pool is full
; Evidence: Obj_New with handler Stage1_FireFlickerRun; called at $020D68 in Scr2_020CF6 right after Stage1_BombingStartFire; TAS f2112
; ---------------------------------------------------------------------------
Stage1_StartFireFlicker:
	lea.l	(Stage1_FireFlickerRun).l,a0
	jsr	(Obj_New).l
	bne.s	loc_020DEE
	clr.l	$18(a6)
	moveq	#$0,d0
loc_020DEE:
	rts
; ---------------------------------------------------------------------------
; Stage1_FireFlickerRun - run-list handler of the fire flicker: when its random 0-7 frame delay runs out, toggles $1A and loads palette line 1 from $020E1C or $020E3C
; In:  a6 = flicker object (+$18 delay, +$1A toggle)
; Out: via Pal_LoadLine1
; Evidence: Rand_Below(8); lea Pal_FireFlicker (two 32-byte palettes, not an a4 script) + $20 on alternate loads; executed in the TAS and fuzz-01800..05400
; ---------------------------------------------------------------------------
Stage1_FireFlickerRun:
	subq.w	#$1,$18(a6)
	bpl.s	loc_020E1A
	move.w	#$8,d0
	jsr	(Rand_Below).l
	move.w	d0,$18(a6)
	lea.l	(Pal_FireFlicker).l,a0
	not.w	$1A(a6)
	beq.s	loc_020E14
	lea.l	$20(a0),a0
loc_020E14:
	jmp	(Pal_LoadLine1).l
loc_020E1A:
	rts
