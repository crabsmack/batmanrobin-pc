; ---------------------------------------------------------------------------
; Boot_Reset - reset vector: on a cold start (ports $A10008/$A1000C read 0) runs the standard Sega init (TMSS 'SEGA' write, 24 VDP registers, VRAM fill, Z80 stub, 64 KB RAM clear, CRAM/VSRAM clear, PSG mute), then falls into the region check
; In:  none (reset)
; Out: never returns: bra Boot_CheckRegion with SR = $2700 and all registers 0
; Evidence: vector 1; Tbl_00038E holds $8000/$3FFF/$0100, a0-a4 = $A00000/$A11100/$A11200/$C00000/$C00004, the VDP register bytes $04,$14,$30,$3C..., the 38-byte Z80 program and the PSG bytes $9F/$BF/$DF/$FF; runs once, TAS frame 1; a soft reset (port control already set) skips straight to the region check
; ---------------------------------------------------------------------------
Boot_Reset:
	tst.l	($A10008).l
	bne.s	loc_00030E
	tst.w	($A1000C).l
loc_00030E:
	bne.s	loc_00038C
	lea.l	Tbl_00038E(pc),a5
	movem.w	(a5)+,d5-d7
	movem.l	(a5)+,a0-a4
	move.b	-$10FF(a1),d0
	andi.b	#$F,d0
	beq.s	loc_00032E
	move.l	#$53454741,$2F00(a1)
loc_00032E:
	move.w	(a4),d0
	moveq	#$0,d0
	movea.l	d0,a6
	move.l	a6,usp
	moveq	#$17,d1
loc_000338:
	move.b	(a5)+,d5
	move.w	d5,(a4)
	add.w	d7,d5
	dbra	d1,loc_000338
	move.l	(a5)+,(a4)
	move.w	d0,(a3)
	move.w	d7,(a1)
	move.w	d7,(a2)
loc_00034A:
	btst	d0,(a1)
	bne.s	loc_00034A
	moveq	#$25,d2
loc_000350:
	move.b	(a5)+,(a0)+
	dbra	d2,loc_000350
	move.w	d0,(a2)
	move.w	d0,(a1)
	move.w	d7,(a2)
loc_00035C:
	move.l	d0,-(a6)
	dbra	d6,loc_00035C
	move.l	(a5)+,(a4)
	move.l	(a5)+,(a4)
	moveq	#$1F,d3
loc_000368:
	move.l	d0,(a3)
	dbra	d3,loc_000368
	move.l	(a5)+,(a4)
	moveq	#$13,d4
loc_000372:
	move.l	d0,(a3)
	dbra	d4,loc_000372
	moveq	#$3,d5
loc_00037A:
	move.b	(a5)+,$11(a3)
	dbra	d5,loc_00037A
	move.w	d0,(a2)
	movem.l	(a6),d0-d7/a0-a6
	move.w	#$2700,sr
loc_00038C:
	bra.s	Boot_CheckRegion
