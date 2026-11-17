; ---------------------------------------------------------------------------
; Boot_CheckRegion - compares the console's region (version register $A10001 bits 7-6 mapped through Tbl_RegionByVersion) with the header region string Hdr_Regions; a match continues the boot, anything else draws the 'DEVELOPED FOR USE ONLY WITH ... SYSTEMS.' lockout screen and hangs
; In:  none (from Boot_Reset)
; Out: never returns: Boot_InitIOPorts on a match; otherwise loops forever at $4FE (also, without a message, when the region byte maps to 0)
; Evidence: the TAS (header 'JU', US console) runs it on frame 3 and goes on to Boot_InitIOPorts; the lockout path sets VDP regs $8164/$8230/$8C81/$8F02/$9001, writes CRAM colour $0EEE, expands Art_LockoutFont (1bpp) to VRAM 0 and prints Str_Lockout* lines through Boot_DrawLockoutText; never shown in the corpus
; ---------------------------------------------------------------------------
Boot_CheckRegion:
	tst.w	($C00004).l
	clr.l	d0
	move.b	($A10001).l,d0
	lsr.b	#$6,d0
	andi.b	#$3,d0
	lea.l	Tbl_RegionByVersion(pc),a0
	move.b	(a0,d0.w),d0
	tst.b	d0
	beq.w	loc_0004FE
	lea.l	(Hdr_Regions).l,a0
	move.w	#$F,d1
loc_000426:
	cmp.b	(a0),d0
	beq.w	Boot_InitIOPorts
	addq.l	#$1,a0
	dbra	d1,loc_000426
	lea.l	($C00000).l,a4
	lea.l	($C00004).l,a5
	move.w	#$8164,(a5)
	move.w	#$8230,(a5)
	move.w	#$8C81,(a5)
	move.w	#$8F02,(a5)
	move.w	#$9001,(a5)
	move.l	#$C0020000,(a5)
	move.w	#$EEE,(a4)
	move.l	#$40000000,(a5)
	lea.l	Art_LockoutFont(pc),a0
	move.w	#$3A,d0
	move.l	#$10000000,d2
loc_000470:
	move.w	#$7,d6
loc_000474:
	move.b	(a0)+,d1
	move.l	#$0,d4
	move.w	#$7,d5
loc_000480:
	rol.l	#$4,d2
	ror.b	#$1,d1
	bcc.s	loc_000488
	or.l	d2,d4
loc_000488:
	dbra	d5,loc_000480
	move.l	d4,(a4)
	dbra	d6,loc_000474
	dbra	d0,loc_000470
	move.b	#$8,d1
	lea.l	Str_LockoutDeveloped(pc),a0
	move.b	(a0)+,d0
	bsr.w	Boot_DrawLockoutText
	lea.l	(Hdr_Regions).l,a1
loc_0004AA:
	cmpi.b	#$20,(a1)
	beq.s	loc_0004F2
	lea.l	Tbl_RegionLockout(pc),a2
loc_0004B4:
	move.w	(a2)+,d4
	tst.b	d4
	beq.s	loc_0004EE
	cmp.b	(a1),d4
	bne.s	loc_0004EA
	cmpi.b	#$20,$1(a1)
	bne.s	loc_0004DA
	cmpa.l	#Hdr_Regions,a1
	beq.s	loc_0004DA
	lea.l	Str_LockoutAmp(pc),a0
	move.b	(a0)+,d0
	addq.w	#$1,d1
	bsr.w	Boot_DrawLockoutText
loc_0004DA:
	lea.l	Tbl_RegionByVersion(pc),a0
	adda.l	(a2)+,a0
	move.b	(a0)+,d0
	addq.w	#$1,d1
	bsr.w	Boot_DrawLockoutText
	bra.s	loc_0004EE
loc_0004EA:
	addq.l	#$4,a2
	bra.s	loc_0004B4
loc_0004EE:
	addq.l	#$1,a1
	bra.s	loc_0004AA
loc_0004F2:
	lea.l	Str_LockoutSystems(pc),a0
	move.b	(a0)+,d0
	addq.w	#$1,d1
	bsr.w	Boot_DrawLockoutText
loc_0004FE:
	bra.s	loc_0004FE
