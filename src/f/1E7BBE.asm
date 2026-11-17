; ---------------------------------------------------------------------------
; Snd_InitSongChannel - fills one Z80 channel record from a song's channel header: the bank/window address of its data, two header bytes, three per-channel bytes from a3, and on/flag bytes $FF (an empty header byte 0 switches the channel off)
; In:  a0 = channel header (byte 0 = length/4), d0 = its 68k address, a1 = Z80 channel record ($A01480+$67n), a3 = per-channel bytes (3 per call)
; Out: d0 = a0 = next channel header (a0 + 4*byte0 + 2); a3 += 3 when on
; Evidence: d0+2 -> lsl.l #1 / lsr.w #1 / +$8000 = Z80 bank number (+$2D) and window address (+$2E/+$2F) of the data after the header; +$2B/+$29 = header bytes 0/1; +$2A/+$20/+$21 from a3; +$3D/+$1F = $FF (0 when off); every byte written with read-back; called 6 times by Snd_StartSongBody; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_InitSongChannel:
	moveq	#$0,d1
	move.b	(a0),d1
	lsl.w	#$2,d1
	beq.w	loc_1E7C56
	movea.l	d0,a2
	addq.l	#$2,d0
	lsl.l	#$1,d0
	lsr.w	#$1,d0
	addi.w	#$8000,d0
loc_1E7BD4:
	move.b	d0,$2F(a1)
	cmp.b	$2F(a1),d0
	bne.s	loc_1E7BD4
	lsr.l	#$8,d0
loc_1E7BE0:
	move.b	d0,$2E(a1)
	cmp.b	$2E(a1),d0
	bne.s	loc_1E7BE0
	lsr.w	#$8,d0
loc_1E7BEC:
	move.b	d0,$2D(a1)
	cmp.b	$2D(a1),d0
	bne.s	loc_1E7BEC
	move.b	(a2)+,d0
loc_1E7BF8:
	move.b	d0,$2B(a1)
	cmp.b	$2B(a1),d0
	bne.s	loc_1E7BF8
	move.b	(a2),d0
loc_1E7C04:
	move.b	d0,$29(a1)
	cmp.b	$29(a1),d0
	bne.s	loc_1E7C04
	move.b	(a3)+,d0
loc_1E7C10:
	move.b	d0,$2A(a1)
	cmp.b	$2A(a1),d0
	bne.s	loc_1E7C10
	move.b	(a3)+,d0
loc_1E7C1C:
	move.b	d0,$20(a1)
	cmp.b	$20(a1),d0
	bne.s	loc_1E7C1C
	move.b	(a3)+,d0
loc_1E7C28:
	move.b	d0,$21(a1)
	cmp.b	$21(a1),d0
	bne.s	loc_1E7C28
	move.b	#$FF,d0
loc_1E7C36:
	move.b	d0,$3D(a1)
	cmp.b	$3D(a1),d0
	bne.s	loc_1E7C36
	move.b	#$FF,d0
loc_1E7C44:
	move.b	d0,$1F(a1)
	cmp.b	$1F(a1),d0
	bne.s	loc_1E7C44
	adda.w	d1,a0
loc_1E7C50:
	addq.w	#$2,a0
	move.l	a0,d0
	rts
loc_1E7C56:
	move.b	d1,d0
loc_1E7C58:
	move.b	d0,$1F(a1)
	cmp.b	$1F(a1),d0
	bne.s	loc_1E7C58
	move.b	d1,d0
loc_1E7C64:
	move.b	d0,$3D(a1)
	cmp.b	$3D(a1),d0
	bne.s	loc_1E7C64
	bra.s	loc_1E7C50
; ---------------------------------------------------------------------------
; Snd_StartSongFinish - tail of Snd_StartSongBody: copies two 3-byte bank pointers from the module header (bytes 7-9, 13-15, byte order 7,9,8) to Z80 RAM $0E61-$0E66, then pulses the Z80 reset and releases the bus so the driver restarts with the new song
; In:  (sp) = the Tbl_1E7EE4 entry pointer pushed by Snd_StartSongBody
; Out: Z80 running; a0/a1 trashed
; Evidence: movea.l 4(a0),a0 = module header ($1E8000/$1F0000/$1F8000) + 7; $A11200 = 0 / $A11100 = 0 / $A11200 = $100; reached by bra.w from Snd_StartSongBody; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_StartSongFinish:
	movea.l	(a7)+,a0
	lea.l	($A00E61).l,a1
	movea.l	$4(a0),a0
	addq.w	#$7,a0
	move.b	(a0)+,(a1)+
	move.b	(a0)+,$1(a1)
	move.b	(a0)+,(a1)
	addq.w	#$3,a0
	addq.w	#$2,a1
	move.b	(a0)+,(a1)+
	move.b	(a0)+,$1(a1)
	move.b	(a0)+,(a1)
	move.w	#$0,($A11200).l
	move.w	#$0,($A11100).l
	move.w	#$100,($A11200).l
	rts
; ---------------------------------------------------------------------------
; Snd_LoadDriverBody - copies the Zyrinx Z80 program Snd_1E6104 ($1825 bytes) to Z80 RAM $A00000 byte by byte with read-back, stores d1 at Z80 $003B, then pulses the Z80 reset and releases the bus
; In:  d1.b = byte for Z80 RAM $003B ($E4 from Snd_Init; its meaning inside the driver not traced)
; Out: Z80 restarted; d0/d2/a0/a5 trashed
; Evidence: lea Snd_1E6104(pc),a5 / $A00000 / dbra #$1824 with cmp.b verify; $A11100/$A11200 = $100, then $A11200 = 0, $A11100 = 0, $A11200 = $100; entered via Snd_LoadDriver (jsr (a1)); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_LoadDriverBody:
	lea.l	Snd_1E6104(pc),a5
	lea.l	($A00000).l,a0
	move.w	#$100,($A11100).l
	move.w	#$100,($A11200).l
loc_1E7CC6:
	btst	#$0,($A11100).l
	bne.s	loc_1E7CC6
	move.w	#$1824,d2
loc_1E7CD4:
	move.b	(a5)+,d0
loc_1E7CD6:
	move.b	d0,(a0)
	cmp.b	(a0),d0
	bne.s	loc_1E7CD6
	addq.w	#$1,a0
	dbra	d2,loc_1E7CD4
	lea.l	($A0003B).l,a0
loc_1E7CE8:
	move.b	d1,(a0)
	cmp.b	(a0),d1
	bne.s	loc_1E7CE8
	move.w	#$0,($A11200).l
	move.w	#$0,($A11100).l
	move.w	#$100,($A11200).l
	rts
