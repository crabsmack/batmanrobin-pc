; ---------------------------------------------------------------------------
; Snd_Init - boot-time sound init: releases the Z80 reset, loads the driver with d1 = $E4, switches every music channel off and resets the fade
; In:  none
; Out: Z80 running the Zyrinx driver, silent; d0/d1/d2/d7/a0/a5 trashed
; Evidence: only caller the main init $9E02 (after the RAM clear and VDP_ClearVRAM); move.w #$100,$A11200 then Snd_LoadDriver, Snd_StopChannels, bra Snd_ResetFade; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_Init:
	move.w	#$100,($A11200).l
	move.w	#$E4,d1
	bsr.w	Snd_LoadDriver
	bsr.w	Snd_StopChannels
	bra.w	Snd_ResetFade
