; ---------------------------------------------------------------------------
; Pal_LoadLine3 - copies a 16-colour palette into line 3 of g_PalSource and flags the line for Fade_Step to rebuild
; In:  a0 = 16 CRAM words
; Out: g_PalSource+$60 ($E132), g_Pal3Reload = $FF; a0 += 32, a1 trashed
; Evidence: 8 longs to $E132; callers Op2_LoadPalette3, the a6 op $7186, $145BE, $1E70A and the stage inits; TAS 30 frames
; ---------------------------------------------------------------------------
Pal_LoadLine3:
	lea.l	(g_PalSource3).l,a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	st.b	(g_Pal3Reload).w
	rts
