; ---------------------------------------------------------------------------
; Snd_StartSongBody - stops the Z80 (bus + reset), finds song d0's header through the module's 3-byte song pointers and initialises the six channel records $A01480+$67n from it
; In:  a0 = Tbl_1E7EE4 entry, d0 = song in the module
; Out: falls into Snd_StartSongFinish (bank pointers, Z80 restart)
; Evidence: song header = 24-bit big-endian pointer at (a0) + 3*d0; a3 = $1E7914 (Snd_1E6104+$1810: 3 bytes per channel copied to +$2A/+$20/+$21); 6 x Snd_InitSongChannel; Z80 reset $A11200 asserted while loading; entered via Snd_StartSong; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_StartSongBody:
	move.l	a0,-(a7)
	lea.l	Snd_1E6104+$1810(pc),a3
	move.w	#$100,($A11100).l
	move.w	#$100,($A11200).l
loc_1E7B86:
	btst	#$0,($A11100).l
	bne.s	loc_1E7B86
	movea.l	(a0),a0
	adda.w	d0,a0
	add.w	d0,d0
	adda.w	d0,a0
	moveq	#$0,d0
	move.b	(a0)+,d0
	lsl.w	#$8,d0
	move.b	(a0)+,d0
	lsl.l	#$8,d0
	move.b	(a0)+,d0
	movea.l	d0,a0
	lea.l	($A01480).l,a1
	moveq	#$5,d7
loc_1E7BAE:
	bsr.w	Snd_InitSongChannel
	adda.w	#$67,a1
	dbra	d7,loc_1E7BAE
	bra.w	Snd_StartSongFinish
