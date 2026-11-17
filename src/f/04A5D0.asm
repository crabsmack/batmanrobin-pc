; ---------------------------------------------------------------------------
; Ending_SpawnEpilogueNarrator - spawns the epilogue's narrator object (Scr2_04A5DC): its coroutine waits 60 frames, types the text at $4ABDE, waits $7080 ticks and ends the scene; its main loop runs Ending_EpilogueTextStep every frame
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A5DC: SetSpeed $100, StartCoroutine0Rel -> $4A602 (SetTimer0 $3C0, TextMap_RunInline $0004ABDE, SetTimer0 $7080, Scene_EndAfterDelay 0), loop StepCoroutine0 / Op2_Native; f19910
; ---------------------------------------------------------------------------
Ending_SpawnEpilogueNarrator:
	lea.l	(Scr2_04A5DC).l,a2
	jmp	(Obj_SpawnEnemy).l
