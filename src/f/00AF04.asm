; ---------------------------------------------------------------------------
; VInt_ScrollDisplayOn - scroll mode 0 and the common tail of every mode: turns the display back on with DMA off (VDP reg 1 = $64)
; In:  a0 = VDP control port
; Out: reg 1 = $64
; Evidence: `move.w #$8164,(a0); rts`; VInt_Handler writes $8134 (display off, DMA on) on entry; target of mode 0 in both tables and of the busy entries; 124 corpus runs, TAS frames 1299-113074
; ---------------------------------------------------------------------------
VInt_ScrollDisplayOn:
	move.w	#$8164,(a0)
	rts
; ---------------------------------------------------------------------------
; VInt_ScrollCustom - scroll mode 4: jumps to the scene's own V-int scroll routine at g_ScrollVIntFn
; In:  a0/a1 = VDP control/data ports; g_ScrollVIntFn = routine
; Out: whatever the routine does (it must restore reg 1)
; Evidence: `movea.l $E55C,a4; jmp (a4)`; $3BB88 sets mode 4 with $E55C = $3BBAC / $3BC56 / $3BCEC; 4 corpus runs
; ---------------------------------------------------------------------------
VInt_ScrollCustom:
	movea.l	(g_ScrollVIntFn).w,a4
	jmp	(a4)
; ---------------------------------------------------------------------------
; VInt_ScrollPlanes - scroll mode 8: whole-plane scroll, writes g_HScrollA/B as one long to the H-scroll table (VRAM $BC00) and g_VScrollA/B as one long to VSRAM 0
; In:  a0/a1 = VDP control/data ports
; Out: reg 1 = $64
; Evidence: commands $7C000002 (VRAM write $BC00) and $40000010 (VSRAM write 0); mode table entry 8 ($AE84); 21 corpus runs
; ---------------------------------------------------------------------------
VInt_ScrollPlanes:
	move.l	#$7C000002,($C00004).l
	move.l	(g_HScrollA).w,(a1)
	move.l	#$40000010,(a0)
	move.l	(g_VScrollA).w,(a1)
	move.w	#$8164,(a0)
	rts
; ---------------------------------------------------------------------------
; VInt_ScrollLines - scroll mode $C: VSRAM long from g_VScrollA/B, then the per-line H-scroll of both planes by DMA (Scroll_DMALines)
; In:  a0/a1 = VDP control/data ports, a2 = DMA command scratch ($FF9910)
; Out: reg 1 = $64
; Evidence: set by Scroll_RunParallax ($97F0) and stage loops; entry $C ($AE88); 92 corpus runs, the most used mode
; ---------------------------------------------------------------------------
VInt_ScrollLines:
	move.l	#$40000010,(a0)
	move.l	(g_VScrollA).w,(a1)
	bsr.w	Scroll_DMALines
	move.w	#$8164,(a0)
	rts
; ---------------------------------------------------------------------------
; VInt_ScrollLinePairs - scroll mode $10: VSRAM long, then one DMA of 448 words of interleaved (A, B) line H-scroll pairs from $FFE1D2 (Scroll_DMALinePairs)
; In:  a0/a1 = VDP ports, a2 = DMA command scratch
; Out: reg 1 = $64
; Evidence: entry $10 ($AE8C); 10 corpus runs (fuzz from frame 122400, the stage 4 end; TAS frames 122681-128749 for $AFDE)
; ---------------------------------------------------------------------------
VInt_ScrollLinePairs:
	move.l	#$40000010,(a0)
	move.l	(g_VScrollA).w,(a1)
	bsr.w	Scroll_DMALinePairs
	move.w	#$8164,(a0)
	rts
