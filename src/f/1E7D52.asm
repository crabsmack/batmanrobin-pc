; ---------------------------------------------------------------------------
; Snd_StopChannelsBody - with the Z80 bus held, writes 0 to byte +$1F (channel on flag) of all six Z80 channel records $A01480+$67n
; In:  none
; Out: Z80 bus released; d0/d7/a1 trashed
; Evidence: the same +$1F byte Snd_InitSongChannel sets to $FF for a live channel; 6 records ($67 apart); entered via Snd_StopChannels; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_StopChannelsBody:
	move.w	#$100,($A11100).l
loc_1E7D5A:
	btst	#$0,($A11100).l
	bne.s	loc_1E7D5A
	lea.l	($A01480).l,a1
	moveq	#$5,d7
loc_1E7D6C:
	move.b	#$0,d0
loc_1E7D70:
	move.b	d0,$1F(a1)
	cmp.b	$1F(a1),d0
	bne.s	loc_1E7D70
	adda.w	#$67,a1
	dbra	d7,loc_1E7D6C
	move.w	#$0,($A11100).l
	rts
