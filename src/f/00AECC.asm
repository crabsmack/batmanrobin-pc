; ---------------------------------------------------------------------------
; VInt_BusyScrollTbl8 - busy-path scroll jump table, modes 8-$10 (continues VInt_BusyScrollTbl): all three branch to VInt_ScrollDisplayOn, so a busy V-int uploads no scroll
; In:  as VInt_BusyScroll (a0 = VDP control port)
; Out: reg 1 = $64
; Evidence: bra.w $AF04 x3 at $AECC/$AED0/$AED4, entered by `jmp VInt_BusyScrollTbl(pc,d0.w)` at $AEC0; entry $C ran in 82 corpus runs (computed targets), 8 in 10, $10 in 8; $AED8-$AEEB (modes $14-$24) are never read
; ---------------------------------------------------------------------------
VInt_BusyScrollTbl8:
	bra.w	VInt_ScrollDisplayOn
; ---------------------------------------------------------------------------
; VInt_BusyMode0C - busy-path V-int scroll slot for mode $C: bra.w VInt_ScrollDisplayOn, so a dropped frame uploads no scroll
; In:  as VInt_BusyScroll (a0 = VDP control port)
; Out: VDP reg 1 = $64
; Evidence: bra.w $AF04 at $AED0, entered by jmp VInt_BusyScrollTbl(pc,d0.w) at $AEC0 when g_ScrollMode = $C (82 corpus runs)
; ---------------------------------------------------------------------------
VInt_BusyMode0C:
	bra.w	VInt_ScrollDisplayOn
; ---------------------------------------------------------------------------
; VInt_BusyMode10 - busy-path V-int scroll slot for mode $10: bra.w VInt_ScrollDisplayOn (display back on, no scroll upload)
; In:  as VInt_BusyScroll
; Out: VDP reg 1 = $64
; Evidence: bra.w $AF04 at $AED4, entered by the jmp at $AEC0 when g_ScrollMode = $10 (8 corpus runs)
; ---------------------------------------------------------------------------
VInt_BusyMode10:
	bra.w	VInt_ScrollDisplayOn
