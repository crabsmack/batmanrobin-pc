; ---------------------------------------------------------------------------
; Frame_BeginStageHook - empty per-frame hook: a bare rts called at the end of Frame_BeginStage
; In:  none
; Out: none
; Evidence: the only instruction is rts; only caller Frame_BeginStage ($9B86); the 38 bytes after it ($15676-$1569B) are never read; next to the debug pause code (inferred: a stripped debug hook)
; ---------------------------------------------------------------------------
Frame_BeginStageHook:
	rts
