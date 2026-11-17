; ---------------------------------------------------------------------------
; BatmobileIntro_SpawnAnimator - spawns the Batmobile scene's object Scr2_04A1A8: ends the scene after 120 frames and runs the native $4A1C8 each frame (rotates 4 colour triples of palette line 0 every 4 frames and, 2 frames in 6, shifts plane A by -$140 px)
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_04A1A8: Scene_EndAfterDelay 0/$78, Op2_StepVelocity, Op2_ApplyMove, Op2_CallNativeRel 6 -> $4A1C8; TAS f503
; ---------------------------------------------------------------------------
BatmobileIntro_SpawnAnimator:
	lea.l	(Scr2_04A1A8).l,a2
	jmp	(Obj_SpawnEnemy).l
