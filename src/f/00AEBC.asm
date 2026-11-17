; ---------------------------------------------------------------------------
; VInt_BusyScroll - the busy-path V-int's scroll step: jumps through VInt_BusyScrollTbl by g_ScrollMode, whose entries turn the display back on, and for modes $28 and $34-$3C also re-arm the H-int raster (worker 13)
; In:  a0 = VDP control port (VInt_Handler set reg 1 = $34)
; Out: reg 1 = $64
; Evidence: jmp VInt_BusyScrollTbl(pc,d0.w); entries 0 and 8-$10 ($AECC-$AED4, next file) all go to $AF04 (`move.w #$8164,(a0)`); only from VInt_BusyPath; 86 TAS frames
; ---------------------------------------------------------------------------
VInt_BusyScroll:
	move.w	(g_ScrollMode).w,d0
	jmp	VInt_BusyScrollTbl(pc,d0.w)
