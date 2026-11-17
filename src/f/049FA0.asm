; ---------------------------------------------------------------------------
; Copyright_DrawText - draws the copyright notice with the text map Tbl_049FB8 (BATMAN AND ALL RELATED ELEMENTS / ARE THE PROPERTY OF DC COMICS / TM & (C)1995 / (C)1995 SEGA) and sends it to plane A
; In:  a2 script context (Op2_Native at $49F9E)
; Out: jsr $7FE.w (Op2_Kill: the object ends)
; Evidence: TextMap_Run Tbl_049FB8 (text bytes), TextMap_Upload; ran once per attract pass (TAS f164)
; ---------------------------------------------------------------------------
Copyright_DrawText:
	lea.l	(Tbl_049FB8).l,a0
	jsr	(TextMap_Run).l
	jsr	(TextMap_Upload).l
	jsr	($7FE).w
