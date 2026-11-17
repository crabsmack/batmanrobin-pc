; ---------------------------------------------------------------------------
; ScrollJt_LayerA - parallax command 4: jump-table slot that branches to ScrollCmd_LayerA
; In:  a5 = after the command word
; Out: see ScrollCmd_LayerA
; Evidence: 4 bytes after the jump-table base $97FC in Scroll_NextCmd; Tbl_00970A starts with command 4; TAS f6362-f101674
; ---------------------------------------------------------------------------
ScrollJt_LayerA:
	bra.w	ScrollCmd_LayerA
