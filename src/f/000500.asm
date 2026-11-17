; ---------------------------------------------------------------------------
; Boot_DrawLockoutText - writes one 0-terminated lockout-screen string to the plane at VRAM $C000 (64 cells wide) at row d1, column d0, one tile per character (tile = char - $20)
; In:  a0 = string, d0.b = column, d1.b = row, a4 = VDP data port, a5 = VDP control port
; Out: a0 past the terminator; d2/d3 clobbered
; Evidence: VDP address = $40000003 + row<<23 + column<<17 (VRAM $C000 + row*$80 + column*2); callers in Boot_CheckRegion pass the first string byte as the column (Str_LockoutDeveloped x=$06 ...) and step d1 per line; never run in the corpus (lockout only)
; ---------------------------------------------------------------------------
Boot_DrawLockoutText:
	move.b	d1,d2
	andi.l	#$FF,d2
	swap	d2
	lsl.l	#$7,d2
	move.b	d0,d3
	andi.l	#$FF,d3
	swap	d3
	asl.l	#$1,d3
	add.l	d3,d2
	addi.l	#$40000003,d2
	move.l	d2,(a5)
loc_000522:
	tst.b	(a0)
	beq.s	loc_000534
	move.b	(a0)+,d2
	subi.b	#$20,d2
	andi.w	#$FF,d2
	move.w	d2,(a4)
	bra.s	loc_000522
loc_000534:
	rts
