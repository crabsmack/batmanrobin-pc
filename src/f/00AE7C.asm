; ---------------------------------------------------------------------------
; VInt_ScrollModeTbl - jump table of the V-int scroll modes 0-$10 (bra.w per 4 bytes): 0 display on only, 4 jmp via the pointer $E55C, 8 H/V scroll longs, $C VSRAM + line-scroll DMA, $10 VSRAM + $AFDE
; In:  as VInt_UploadScroll
; Out: as the mode routine
; Evidence: targets $AF04/$AF0A/$AF10/$AF2E/$AF42; entries for $14-$24 ($AE90-$AEA3) are never read in the corpus; mode 0 entered in TAS frames 21158-35416
; ---------------------------------------------------------------------------
VInt_ScrollModeTbl:
	bra.w	VInt_ScrollDisplayOn
; ---------------------------------------------------------------------------
; VInt_ScrollMode04 - V-int scroll jump-table slot for mode 4: bra.w VInt_ScrollCustom (a scene's own upload through the pointer at $E55C)
; In:  as VInt_UploadScroll (a0 = VDP control port, a1 = data port)
; Out: as VInt_ScrollCustom
; Evidence: bra.w $AF0A at $AE80, entered by jmp VInt_ScrollModeTbl(pc,d0.w) at $AE78 when g_ScrollMode = 4 (computed-jump target in the corpus)
; ---------------------------------------------------------------------------
VInt_ScrollMode04:
	bra.w	VInt_ScrollCustom
; ---------------------------------------------------------------------------
; VInt_ScrollMode08 - V-int scroll jump-table slot for mode 8: bra.w VInt_ScrollPlanes (one H-scroll long and one V-scroll long)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollPlanes
; Evidence: bra.w $AF10 at $AE84, entered by the jmp at $AE78 when g_ScrollMode = 8
; ---------------------------------------------------------------------------
VInt_ScrollMode08:
	bra.w	VInt_ScrollPlanes
; ---------------------------------------------------------------------------
; VInt_ScrollMode0C - V-int scroll jump-table slot for mode $C: bra.w VInt_ScrollLines (VSRAM long plus the per-line H-scroll DMA)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollLines
; Evidence: bra.w $AF2E at $AE88, entered by the jmp at $AE78 when g_ScrollMode = $C (the gameplay default)
; ---------------------------------------------------------------------------
VInt_ScrollMode0C:
	bra.w	VInt_ScrollLines
; ---------------------------------------------------------------------------
; VInt_ScrollMode10 - V-int scroll jump-table slot for mode $10: bra.w VInt_ScrollLinePairs (VSRAM long plus the line-pair H-scroll DMA)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollLinePairs
; Evidence: bra.w $AF42 at $AE8C, entered by the jmp at $AE78 when g_ScrollMode = $10
; ---------------------------------------------------------------------------
VInt_ScrollMode10:
	bra.w	VInt_ScrollLinePairs
