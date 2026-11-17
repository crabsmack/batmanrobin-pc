; ---------------------------------------------------------------------------
; Layer_ScrollStep - per-frame layer scroll: applies up to 16 px of the pending x (+4) then of the pending y (+6), queueing the newly exposed block column / row
; In:  a6 = layer
; Out: layer position, +4/+6 reduced, g_TileQueue extended
; Evidence: called from 10 stage loops ($01FC50, $01FECC, $033398, $03FE94, ...) every frame with a6 = g_LayerA; the pending amounts come from Camera_TrackPlayersXLayerA
; ---------------------------------------------------------------------------
Layer_ScrollStep:
	bsr.w	Layer_ScrollStepX
	bra.w	Layer_ScrollStepY
