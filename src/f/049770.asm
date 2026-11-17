; ---------------------------------------------------------------------------
; Title_DimPicture - darkens the title picture behind the menu text: sets the red, green and blue tints of palette lines 0-2 to -4
; In:  -
; Out: g_Pal0-2Tint{R,G,B} = -4
; Evidence: Op2_CallNativeRel from $4972A when the menu opens (TAS f404, scenario-options f404 and f1449); png x-options f420/f600: the logo dimmed under the OPTIONS text
; ---------------------------------------------------------------------------
Title_DimPicture:
	moveq	#-$4,d0
	move.b	d0,(g_Pal0TintR).w
	move.b	d0,(g_Pal1TintR).w
	move.b	d0,(g_Pal2TintR).w
	move.b	d0,(g_Pal0TintG).w
	move.b	d0,(g_Pal1TintG).w
	move.b	d0,(g_Pal2TintG).w
	move.b	d0,(g_Pal0TintB).w
	move.b	d0,(g_Pal1TintB).w
	move.b	d0,(g_Pal2TintB).w
	rts
