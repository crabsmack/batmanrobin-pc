; ---------------------------------------------------------------------------
; VInt_ScrollModeTbl28 - jump table of the V-int scroll modes $28-$3C, which mostly end in the H-int raster setup ($B1E0)
; In:  as VInt_UploadScroll
; Out: as the mode routine
; Evidence: continues VInt_ScrollModeTbl at offset $28: $AF84 ($B03E then $B1E0), $AF8C, $AF9A, $AFA4, $AFAC, $AFBE; set by scenes through Scroll_SetMode and $1FECC/$3BB88 (g_ScrollMode comment); 8 corpus runs, TAS frames 113312-116145
; ---------------------------------------------------------------------------
VInt_ScrollModeTbl28:
	bra.w	VInt_ScrollLinesARaster
; ---------------------------------------------------------------------------
; VInt_ScrollMode2C - V-int scroll jump-table slot for mode $2C: bra.w VInt_ScrollLinesColumns (line H-scroll plus column V-scroll DMAs)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollLinesColumns
; Evidence: bra.w $AF8C at $AEA8, entered by the jmp at $AE78 when g_ScrollMode = $2C
; ---------------------------------------------------------------------------
VInt_ScrollMode2C:
	bra.w	VInt_ScrollLinesColumns
; ---------------------------------------------------------------------------
; VInt_ScrollMode30 - V-int scroll jump-table slot for mode $30: bra.w VInt_ScrollLinesA (plane A line H-scroll DMA only)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollLinesA
; Evidence: bra.w $AF9A at $AEAC, entered by the jmp at $AE78 when g_ScrollMode = $30
; ---------------------------------------------------------------------------
VInt_ScrollMode30:
	bra.w	VInt_ScrollLinesA
; ---------------------------------------------------------------------------
; VInt_ScrollMode34 - V-int scroll jump-table slot for mode $34: bra.w VInt_ScrollLinesBRaster (plane B line DMA, then the H-int raster setup)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollLinesBRaster
; Evidence: bra.w $AFA4 at $AEB0, entered by the jmp at $AE78 when g_ScrollMode = $34
; ---------------------------------------------------------------------------
VInt_ScrollMode34:
	bra.w	VInt_ScrollLinesBRaster
; ---------------------------------------------------------------------------
; VInt_ScrollMode38 - V-int scroll jump-table slot for mode $38: bra.w VInt_ScrollLinesRaster (VSRAM long, line DMA, H-int raster setup)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollLinesRaster
; Evidence: bra.w $AFAC at $AEB4, entered by the jmp at $AE78 when g_ScrollMode = $38
; ---------------------------------------------------------------------------
VInt_ScrollMode38:
	bra.w	VInt_ScrollLinesRaster
; ---------------------------------------------------------------------------
; VInt_ScrollMode3C - V-int scroll jump-table slot for mode $3C: bra.w VInt_ScrollPlanesRaster (the plane scroll longs, then the H-int raster setup)
; In:  as VInt_UploadScroll
; Out: as VInt_ScrollPlanesRaster
; Evidence: bra.w $AFBE at $AEB8, entered by the jmp at $AE78 when g_ScrollMode = $3C
; ---------------------------------------------------------------------------
VInt_ScrollMode3C:
	bra.w	VInt_ScrollPlanesRaster
