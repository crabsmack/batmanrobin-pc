; ---------------------------------------------------------------------------
; Pal_LoadLine0 - copies a 16-colour palette into line 0 of g_PalSource and flags the line for Fade_Step to rebuild
; In:  a0 = 16 CRAM words
; Out: g_PalSource line 0 ($E0D2), g_Pal0Reload = $FF; a0 += 32, a1 trashed
; Evidence: 8 longs to $E0D2; callers: the a6 scene op $7186 (palette by line bits), stage inits ($1FB3A, $1FECC, $32F14, $3FE94, $475C2 ...); TAS 25 frames
; ---------------------------------------------------------------------------
Pal_LoadLine0:
	lea.l	(g_PalSource).l,a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	st.b	(g_Pal0Reload).w
	rts
