; ---------------------------------------------------------------------------
; Pal_LoadLine1 - copies a 16-colour palette into line 1 of g_PalSource and flags the line for Fade_Step to rebuild
; In:  a0 = 16 CRAM words
; Out: g_PalSource+$20 ($E0F2), g_Pal1Reload = $FF; a0 += 32, a1 trashed
; Evidence: 8 longs to $E0F2; callers Op2_LoadPalette1, the a6 op $7186, $20DDA, $497DA; TAS 967 frames
; ---------------------------------------------------------------------------
Pal_LoadLine1:
	lea.l	(g_PalSource1).l,a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	st.b	(g_Pal1Reload).w
	rts
