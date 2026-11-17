; ---------------------------------------------------------------------------
; ScrollJt_CameraA - parallax command $C: jump-table slot that branches to ScrollCmd_CameraA
; In:  a5 = after the command word
; Out: see ScrollCmd_CameraA
; Evidence: offset $C from the jump-table base $97FC; ran only TAS f108331-f113271 (stage 4)
; ---------------------------------------------------------------------------
ScrollJt_CameraA:
	bra.w	ScrollCmd_CameraA
