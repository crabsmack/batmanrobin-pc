; ---------------------------------------------------------------------------
; Pal_LoadLine2 - copies a 16-colour palette into line 2 of g_PalSource and flags the line for Fade_Step to rebuild
; In:  a0 = 16 CRAM words
; Out: g_PalSource+$40 ($E112), g_Pal2Reload = $FF; a0 += 32, a1 trashed
; Evidence: 8 longs to $E112; callers Op2_LoadPalette2, the a6 op $7186, $1FB3A, $1FC50, $497DA; TAS 23 frames
; ---------------------------------------------------------------------------
Pal_LoadLine2:
	lea.l	(g_PalSource2).l,a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	st.b	(g_Pal2Reload).w
	rts
