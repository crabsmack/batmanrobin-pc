; ---------------------------------------------------------------------------
; Debug_DrawHexBytes - draws d1 bytes from a6 as hex digit sprites straight into the SAT at a character-cell position
; In:  a6 = bytes, d1 = byte count, d0 = position (high byte column, low byte row, 8 px each, +$20)
; Out: a6 past the bytes
; Evidence: per nibble Spr_ChangeFrame to frame nibble*$A + $A (digit frames) and Spr_DrawDirect, x += 8; caller Debug_FreeCamStep (9 readouts); static only
; ---------------------------------------------------------------------------
Debug_DrawHexBytes:
	move.w	d0,d5
	andi.w	#$FF,d5
	lsl.w	#$3,d5
	addi.w	#$20,d5
	move.w	d0,d4
	andi.w	#$FF00,d4
	lsr.w	#$5,d4
	addi.w	#$20,d4
loc_015958:
	move.w	d1,-(a7)
	move.b	(a6),d1
	andi.w	#$F0,d1
	lsr.w	#$4,d1
	mulu.w	#$A,d1
	addi.w	#$A,d1
	move.w	(g_DebugHexFrame).w,d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(g_DebugHexFrame).w
	move.w	#$8000,d6
	jsr	(Spr_DrawDirect).l
	addq.w	#$8,d4
	move.b	(a6)+,d1
	andi.w	#$F,d1
	mulu.w	#$A,d1
	addi.w	#$A,d1
	move.w	(g_DebugHexFrame).w,d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(g_DebugHexFrame).w
	move.w	#$8000,d6
	jsr	(Spr_DrawDirect).l
	addq.w	#$8,d4
	move.w	(a7)+,d1
	subq.w	#$1,d1
	bne.s	loc_015958
	rts
; ---------------------------------------------------------------------------
; Stage1_VIntFireStep - V-int step of the stage 1 street's building fire on plane A: state 2 blanks the top 6 rows of plane A and moves to 1; state 1 DMAs the next fire frame's tiles and writes its map rows, cycling through the frame list Tbl_015A7E
; In:  d0 = fire state ($F518: 1 or 2), a0/a1 = VDP control/data (V-int)
; Out: state 2: $F518 = 1, VRAM $C000 filled with tile $5FC (384 longs); state 1: $F4F8 = the frame's x offset, $F4F4 = next 20-byte entry (back to Tbl_015A7E at a loop flag)
; Evidence: entry = word source offset (in the $E09E art buffer), long tile DMA command, word rows-1, word tile attr, word map offset, long map VDP command ($40000003 = plane A $C000), word x offset, word loop flag; called by Stage1_VIntFacade while $F518 > 0; the fire burns over the bank from TAS f2160 (screenshots f2400-f4200)
; ---------------------------------------------------------------------------
Stage1_VIntFireStep:
	subq.b	#$1,d0
	beq.s	loc_0159E4
	move.b	#$1,(g_Stage1FireState).w
	move.l	#$40000003,(a0)
	move.l	#$5FC05FC,d0
	move.w	#$2F,d7
loc_0159CE:
	move.l	d0,(a1)
	move.l	d0,(a1)
	move.l	d0,(a1)
	move.l	d0,(a1)
	move.l	d0,(a1)
	move.l	d0,(a1)
	move.l	d0,(a1)
	move.l	d0,(a1)
	dbra	d7,loc_0159CE
	rts
loc_0159E4:
	movea.l	(g_SceneVars).w,a3
	move.l	#$93909401,(a0)
	movea.l	($FFFFE09E).l,a4
	adda.l	(a4),a4
	adda.w	(a3)+,a4
	move.l	a4,d0
	lsr.l	#$1,d0
	move.w	d0,d1
	andi.l	#$7F00FF,d0
	ori.l	#$97009500,d0
	lsr.w	#$8,d1
	ori.w	#$9600,d1
	move.l	d0,(a0)
	move.w	d1,(a0)
	move.l	(a3)+,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.w	(a3)+,d7
	move.w	(a3)+,d2
	movea.l	($FFFFE09E).l,a4
	addq.l	#$4,a4
	adda.l	(a4),a4
	adda.w	(a3)+,a4
	move.l	(a3)+,d1
	move.w	d2,d3
	swap	d2
	move.w	d3,d2
loc_015A34:
	move.l	d1,(a0)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	move.l	(a4)+,d0
	add.l	d2,d0
	move.l	d0,(a1)
	addi.l	#$1000000,d1
	dbra	d7,loc_015A34
	move.w	(a3)+,(g_SceneVars+$4).w
	tst.w	(a3)+
	beq.s	loc_015A78
	lea.l	(Tbl_015A7E).l,a3
loc_015A78:
	move.l	a3,(g_SceneVars).w
	rts
