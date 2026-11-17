; ---------------------------------------------------------------------------
; VInt_UploadScroll - the V-int's scroll upload: jumps through VInt_ScrollModeTbl by g_ScrollMode; every mode ends by turning the display back on (VDP reg 1 = $64)
; In:  a0 = VDP control port, a1 = data port (from VInt_Handler, which set reg 1 = $34: display off during the V-int)
; Out: plane scroll values / line tables sent per mode; reg 1 = $64
; Evidence: jmp VInt_ScrollModeTbl(pc,d0.w); mode 8 ($AF10) writes g_HScrollA as one long to VRAM $BC00 and g_VScrollA to VSRAM 0, $C ($AF2E) DMAs the line buffers (Scroll_RunParallax); bsr from VInt_Handler every full V-int (130,055 TAS frames)
; ---------------------------------------------------------------------------
VInt_UploadScroll:
	move.w	(g_ScrollMode).w,d0
	jmp	VInt_ScrollModeTbl(pc,d0.w)
