; ---------------------------------------------------------------------------
; Stage1_StepSignDebris - moves one sign-debris piece (x += vx, vy += $7000 gravity, y += vy), advances its 6-frame tumble by the rate accumulator, and draws it in front
; In:  a5 = piece, a6 = debris object (+$18 shared frame slot)
; Out: a5 += $18
; Evidence: Spr_ChangeFrame on $18(a6) with frame = base $14 + (0..$32 step $A), Spr_AddFront; called 3 times per sprite block by Stage1_SignDebrisRun (TAS f1830-f4352)
; ---------------------------------------------------------------------------
Stage1_StepSignDebris:
	move.l	(a5),d4
	add.l	d4,$4(a5)
	addi.l	#$7000,$8(a5)
	move.l	$8(a5),d5
	add.l	d5,$C(a5)
	move.w	$12(a5),d1
	move.b	$10(a5),d0
	add.b	d0,$11(a5)
	bcc.s	loc_020A7E
	addi.w	#$A,d1
	cmpi.w	#$3C,d1
	blt.s	loc_020A7A
	subi.w	#$3C,d1
loc_020A7A:
	move.w	d1,$12(a5)
loc_020A7E:
	add.w	$14(a5),d1
	move.w	$18(a6),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,$18(a6)
	move.w	$4(a5),d4
	move.w	$C(a5),d5
	move.w	$16(a5),d6
	jsr	(Spr_AddFront).l
	lea.l	$18(a5),a5
	rts
; ---------------------------------------------------------------------------
; Stage1_StartRubbleRain - bombing native: starts a run-list object that rains rubble pieces on the street for $578 frames (Stage1_RubbleRainRun), with the piece records of Tbl_020C26
; In:  a6 = the bombing object
; Out: d0 = 0 (d0 = -1, nothing when the pool is full)
; Evidence: Obj_NewClear with handler Stage1_RubbleRainRun, $1C = Tbl_020C26, $18 = $578, $1A = 6; called by Op2_CallNativeRelSaved at $020D3E in Scr2_020CF6; executed in the TAS and fuzz-01800 (around f1989)
; ---------------------------------------------------------------------------
Stage1_StartRubbleRain:
	lea.l	(Stage1_RubbleRainRun).l,a0
	jsr	(Obj_NewClear).l
	bne.s	loc_020ACC
	move.l	#$20C26,$1C(a6)
	move.w	#$578,$18(a6)
	move.w	#$6,$1A(a6)
	moveq	#$0,d0
loc_020ACC:
	rts
; ---------------------------------------------------------------------------
; Stage1_RubbleRainRun - run-list handler of the rubble rain: every 7 frames spawns a rubble piece (Stage1_RubblePieceRun) at a random x within +-150 px of the screen centre at the camera top, cycling through the Tbl_020C26 records; frees itself when $18 runs out
; In:  a6 = rain object (+$18 frames left, +$1A spawn delay, +$1C record cursor)
; Out: new piece: +$20/$24 x/y, +$28/$2C x speed/accel, +$30/$34 y speed/gravity, +$38 fall frames, +$3A animation table
; Evidence: Rand_Next * $12C + $A00000 + g_CameraX; a zero record pointer wraps to Tbl_020C26; Obj_UnlinkFree at the end; png f002400 shows the pieces falling over the street
; ---------------------------------------------------------------------------
Stage1_RubbleRainRun:
	subq.w	#$1,$18(a6)
	bmi.w	loc_020B4E
	subq.w	#$1,$1A(a6)
	bpl.s	loc_020B4C
	move.w	#$6,$1A(a6)
	movea.l	a6,a5
	lea.l	(Stage1_RubblePieceRun).l,a0
	jsr	(Obj_New).l
	bne.s	loc_020B4C
	jsr	(Rand_Next).l
	muls.w	#$12C,d0
	addi.l	#$A00000,d0
	add.l	(g_CameraX).l,d0
	move.l	d0,$20(a6)
	move.l	(g_CameraY).l,$24(a6)
	movea.l	$1C(a5),a0
	move.l	(a0)+,d0
	bne.s	loc_020B24
	lea.l	(Tbl_020C26).l,a0
	move.l	(a0)+,d0
loc_020B24:
	movea.l	d0,a1
	move.l	a0,$1C(a5)
	move.w	(a1)+,$38(a6)
	move.l	(a1)+,$30(a6)
	move.l	(a1)+,$34(a6)
	move.l	a1,$3A(a6)
	clr.w	$3E(a6)
	clr.w	$40(a6)
	clr.l	$28(a6)
	clr.l	$2C(a6)
	movea.l	a5,a6
loc_020B4C:
	rts
loc_020B4E:
	jmp	(Obj_UnlinkFree).l
; ---------------------------------------------------------------------------
; Stage1_RubblePieceRun - run-list handler of one rubble piece: falls freely while its fall counter runs, then moves with the scene terrain routine, bouncing off the floor at half speed with a random x drift; animates from its table and is freed below y $100
; In:  a6 = piece
; Out: drawn with Spr_AddBack (attr $8000 priority), or released
; Evidence: movea.l g_TerrainFn,a0 / jsr (a0) with d6 = 0, d7 = 3; floor contact: $38 = $32, vy = -vy/2; Spr_ReleaseFrame + Obj_UnlinkFree when y > $100; executed in the TAS and fuzz-01800/03600
; ---------------------------------------------------------------------------
Stage1_RubblePieceRun:
	move.l	$2C(a6),d1
	add.l	$28(a6),d1
	move.l	d1,$28(a6)
	move.l	$34(a6),d2
	add.l	$30(a6),d2
	move.l	d2,$30(a6)
	movem.l	$20(a6),d4-d5
	subq.w	#$1,$38(a6)
	bpl.s	loc_020BC8
	add.l	d4,d1
	add.l	d5,d2
	move.w	d1,$22(a6)
	move.w	d2,$26(a6)
	swap	d1
	swap	d2
	swap	d4
	swap	d5
	sub.w	d4,d1
	sub.w	d5,d2
	moveq	#$0,d6
	move.w	#$3,d7
	movea.l	(g_TerrainFn).w,a0
	jsr	(a0)
	move.w	d4,$20(a6)
	move.w	d5,$24(a6)
	tst.w	d7
	beq.s	loc_020BD8
	move.w	#$32,$38(a6)
	move.l	$30(a6),d0
	asr.l	#$1,d0
	neg.l	d0
	move.l	d0,$30(a6)
	jsr	(Rand_Next).l
	add.l	d0,d0
	move.l	d0,$28(a6)
	bra.s	loc_020BD8
loc_020BC8:
	add.l	d1,d4
	add.l	d2,d5
	move.l	d4,$20(a6)
	move.l	d5,$24(a6)
	swap	d4
	swap	d5
loc_020BD8:
	cmpi.w	#$100,d5
	bgt.s	loc_020C14
	movea.l	$3A(a6),a0
	movem.w	(a0)+,d0-d1
	add.w	$40(a6),d0
	cmp.w	d1,d0
	blt.s	loc_020BF0
	sub.w	d1,d0
loc_020BF0:
	move.w	d0,$40(a6)
	lsr.w	#$8,d0
	add.w	d0,d0
	move.w	(a0,d0.w),d1
	move.w	$3E(a6),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,$3E(a6)
	move.w	#$8000,d6
	jmp	(Spr_AddBack).l
loc_020C14:
	move.w	$3E(a6),d2
	jsr	(Spr_ReleaseFrame).l
	jmp	(Obj_UnlinkFree).l
