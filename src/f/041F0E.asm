; ---------------------------------------------------------------------------
; Enemy_GasOrb_StepJet - one gas jet: when its var-3 bit is clear frees its puff block; when set, allocates it (5 puffs with phases 0, -$1E0 ...) or moves each puff out along the heading, draws it and lets the damaging frames hit players (+$10 damage, hit type 8, knockback 2.0 away), then plays sound $3C
; In:  d0 = jet bit, a2 = a6 field holding the puff block handle, a3/a4/a5/a6 as Enemy_GasOrb_RunJets
; Out: d0-d7/a0-a2 clobbered
; Evidence: puff = 10 bytes: radius.l, speed.l, phase.w (+$80 per frame; (re)starts at radius $28 with speed = cruise y $34(a3) when its negative start delay runs out and again after $8FF; speed += accel y $28(a3)); position = orb x/y + radius * (cos, sin)(heading) via Tbl_059B22; frame and hit flag from the word pairs at Scr2_04204C (frames $497A-$49CA, flag on 3 of 9); 7x7 box Hitbox_FindPlayerBox + Hitbox_CanHitTarget, spark Scr4_010964 via Hitbox_SpawnAtContact; Snd_PlaySfx with $3C; 3 bsr from Enemy_GasOrb_RunJets
; ---------------------------------------------------------------------------
Enemy_GasOrb_StepJet:
	and.w	$28(a4),d0
	bne.s	loc_041F24
	move.w	(a2),d0
	beq.w	loc_041F46
	clr.w	(a2)
	movea.w	d0,a0
	jmp	(ObjPool_Free).l
loc_041F24:
	move.w	(a2),d0
	bne.s	loc_041F48
	jsr	(ObjPool_AllocClear).l
	move.w	a0,(a2)
	moveq	#$0,d0
	moveq	#$0,d1
	moveq	#$4,d7
loc_041F36:
	move.w	d0,$8(a0)
	subi.w	#$1E0,d0
	lea.l	$A(a0),a0
	dbra	d7,loc_041F36
loc_041F46:
	rts
loc_041F48:
	movea.w	d0,a2
	moveq	#$4,d7
loc_041F4C:
	movem.l	d7/a2,-(a7)
	move.w	$8(a2),d0
	addi.w	#$80,d0
	bcs.s	loc_041F6A
	bpl.s	loc_041F64
	move.w	d0,$8(a2)
	bra.w	loc_042034
loc_041F64:
	cmpi.w	#$900,d0
	blt.s	loc_041F78
loc_041F6A:
	moveq	#$0,d0
	move.l	#$280000,d4
	move.l	$34(a3),d5
	bra.s	loc_041F84
loc_041F78:
	move.l	(a2),d4
	move.l	$4(a2),d5
	add.l	$28(a3),d5
	add.l	d5,d4
loc_041F84:
	move.w	d0,$8(a2)
	move.l	d4,(a2)
	move.l	d5,$4(a2)
	swap	d4
	move.w	d4,d5
	move.w	$C(a3),d1
	andi.w	#$3FE,d1
	lea.l	(Tbl_059B22).l,a0
	adda.w	d1,a0
	muls.w	$100(a0),d4
	muls.w	(a0),d5
	swap	d4
	swap	d5
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	lsr.w	#$6,d0
	andi.w	#$FFFC,d0
	lea.l	Scr2_04204C(pc),a0
	move.w	(a0,d0.w),d1
	move.w	$2(a0,d0.w),-(a7)
	move.w	$2A(a6),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,$2A(a6)
	move.w	$38(a5),d6
	move.w	$3A(a5),d7
	jsr	(Spr_AddFront).l
	tst.w	(a7)+
	beq.s	loc_042034
	move.w	d4,d6
	move.w	d5,d7
	subq.w	#$3,d4
	subq.w	#$3,d5
	addq.w	#$3,d6
	addq.w	#$3,d7
	jsr	(Hitbox_FindPlayerBox).l
	beq.s	loc_042034
	jsr	(Hitbox_CanHitTarget).l
	bne.s	loc_042034
	addi.w	#$10,$20(a2)
	move.w	#$8,$1A(a2)
	move.l	#$20000,d2
	move.w	$12(a2),d1
	add.w	$16(a2),d1
	sub.w	d4,d1
	sub.w	d6,d1
	bpl.s	loc_042024
	neg.l	d2
loc_042024:
	move.l	d2,$22(a2)
	lea.l	(Scr4_010964).l,a1
	jsr	(Hitbox_SpawnAtContact).l
loc_042034:
	movem.l	(a7)+,d7/a2
	lea.l	$A(a2),a2
	dbra	d7,loc_041F4C
	move.w	#$3C,d0
	jsr	(Snd_PlaySfx).l
	rts
