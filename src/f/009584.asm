; ---------------------------------------------------------------------------
; Pal_FlashWhite - native callback: flashes the screen white for 4 frames
; In:  none
; Out: g_FlashReq set, g_FlashTimer = 4
; Evidence: $A7F0 fills CRAM with $0EEE while g_FlashReq is set and restores the palette when the timer ends; a4 op $6492 does the same; 21 script uses after op $2CBC ($13788, $1992C...)
; ---------------------------------------------------------------------------
Pal_FlashWhite:
	st.b	(g_FlashReq).w
	move.b	#$4,(g_FlashTimer).w
	rts
