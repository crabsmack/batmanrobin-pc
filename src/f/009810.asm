; ---------------------------------------------------------------------------
; ScrollJt_CameraB - parallax command $14: jump-table slot that branches to ScrollCmd_CameraB
; In:  a5 = after the command word
; Out: see ScrollCmd_CameraB
; Evidence: offset $14 from the jump-table base $97FC; Tbl_00970A's second command is $14; TAS f6362-f119385
; ---------------------------------------------------------------------------
ScrollJt_CameraB:
	bra.w	ScrollCmd_CameraB
; ---------------------------------------------------------------------------
; ScrollJt_CameraOfsB - parallax command $18: jump-table slot that branches to ScrollCmd_CameraOfsB
; In:  a5 = after the command word
; Out: see ScrollCmd_CameraOfsB
; Evidence: offset $18 from the jump-table base $97FC, the bra.w at $9814 inside the $9810 slot pair
; ---------------------------------------------------------------------------
ScrollJt_CameraOfsB:
	bra.w	ScrollCmd_CameraOfsB
