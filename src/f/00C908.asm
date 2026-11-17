; ---------------------------------------------------------------------------
; Mem_FillWords - stores the word d0 d7 times at (a0)+ (up to 224: one per screen line)
; In:  a0 = destination, d0.w = value, d7.w = count (0-224)
; Out: a0 past the words, d7 = -2*count
; Evidence: `add.w d7,d7; neg.w d7; jmp $C906(pc,d7.w)`; callers Scroll_FillBands (each parallax band's lines), $3BB88 (3 jmps), $48656 (224 lines of g_HScrollLinesB); 63 corpus runs, TAS frames 6362-119385
; ---------------------------------------------------------------------------
Mem_FillWords:
	add.w	d7,d7
	neg.w	d7
	jmp	loc_00C906(pc,d7.w)
; ---------------------------------------------------------------------------
; Mem_FillLongsRun - unrolled body of Mem_FillLongs: 112 `move.l d0,(a0)+`
; In:  entered mid-way by `jmp Mem_FillLongsRun+$E0(pc,d7.w)` at $C9F6 with d7 = -2 x count
; Out: as Mem_FillLongs
; Evidence: dkit.toml computed_entry_runs $C910-$C9F0 stride 2; ends at the rts $C9F0
; ---------------------------------------------------------------------------
Mem_FillLongsRun:
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
loc_00C9F0:
	rts
; ---------------------------------------------------------------------------
; Mem_FillLongs - stores the long d0 d7 times at (a0)+ (up to 112 longs = 224 words)
; In:  a0 = destination, d0.l = value, d7.w = count (0-112)
; Out: a0 past the longs, d7 = -2*count
; Evidence: `add.w d7,d7; neg.w d7; jmp $C9F0(pc,d7.w)`; callers $24660 (64 longs of g_HScrollLinesB, 10 of $F526) and $33448 (112 longs of g_HScrollLinesB with the halves swapped on alternate frames)
; ---------------------------------------------------------------------------
Mem_FillLongs:
	add.w	d7,d7
	neg.w	d7
	jmp	loc_00C9F0(pc,d7.w)
