; ---------------------------------------------------------------------------
; Scroll_RunParallax - selects the per-line scroll upload (g_ScrollMode $C) and runs a parallax command list that fills the line H-scroll buffers
; In:  a5 = command list (word command, its operands; 0 ends), e.g. Tbl_00970A: cmd 4 (table $971C, layer g_LayerA), cmd $14 (table $9720)
; Out: g_ScrollMode = $C; the commands' line buffers and V-scroll words
; Evidence: callers are stage frame loops ($1FCDA, $1FDEA, $3FF1E, $4016E, $40454, $49612: `lea Tbl_xxx,a5; jsr $97F0`); mode $C makes the V-int ($AE88 -> $AF2E) DMA $FFE1D2/$FFE392 to VRAM $BC00/$BC02 with stride 4; falls into Scroll_NextCmd; TAS f6362-f113271
; ---------------------------------------------------------------------------
Scroll_RunParallax:
	move.w	#$C,(g_ScrollMode).w
