; ---------------------------------------------------------------------------
; VM6_Run - runs an a6 loader script right now from a scene setup: dispatches its first op; the script runs on until an op parks it for Frame_End (decompress in the background, V-int jobs, queued DMA) or ends
; In:  a6 = script
; Out: returns when the script parks (g_A6ScriptPc/g_A6ResumeHook set) or ends (Op6_End)
; Evidence: jsr from 39 scene/section setups after Scene_InitStage and the palettes (e.g. Stage1_RunStreet with Tbl_01FAB2 `7108 0000 0005 E63C 7604 ...`, $1FC50 Scr6_01FC06, $32F14 Scr6_032E4A); movea.w (a6)+,a0 / jmp (a0); 40 corpus runs
; ---------------------------------------------------------------------------
VM6_Run:
	movea.w	(a6)+,a0
	jmp	(a0)
