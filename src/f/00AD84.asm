; ---------------------------------------------------------------------------
; VDP_ClearVRAM - zeroes all 64 KB of VRAM
; In:  none
; Out: a0 = $C00000, d0 = 0, d7 = -1
; Evidence: VRAM write address 0 ($40000000), $1000 x 16 bytes through VDP_ClearLoop; only caller Main_Start; TAS frame 6, once
; ---------------------------------------------------------------------------
VDP_ClearVRAM:
	lea.l	($C00000).l,a0
	move.w	#$FFF,d7
	move.l	#$40000000,$4(a0)
; ---------------------------------------------------------------------------
; VDP_ClearLoop - shared tail of the VDP clears: sets d0 = 0 and falls into VDP_FillLoop
; In:  a0 = VDP data port with the control port already set (at 4(a0)), d7 = 16-byte units - 1
; Out: as VDP_FillLoop with d0 = 0
; Evidence: bra targets of VDP_ClearCRAM, VDP_ClearVSRAM, VDP_ClearSAT, VDP_ClearHScroll; fall-through of VDP_ClearVRAM
; ---------------------------------------------------------------------------
VDP_ClearLoop:
	moveq	#$0,d0
; ---------------------------------------------------------------------------
; VDP_FillLoop - writes the long d0 four times per iteration to the VDP data port: fills d7+1 16-byte units
; In:  a0 = VDP data port (address already set), d0 = fill long, d7 = units - 1
; Out: d7 = -1
; Evidence: 4 x move.l d0,(a0) + dbra; bra targets of VDP_WhiteCRAM ($0EEE0EEE) and VDP_ClearPlanes ($05FC05FC)
; ---------------------------------------------------------------------------
VDP_FillLoop:
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	move.l	d0,(a0)
	dbra	d7,VDP_FillLoop
	rts
