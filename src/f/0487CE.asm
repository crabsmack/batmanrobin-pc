; ---------------------------------------------------------------------------
; Story_SpawnFacadeCamera - spawns the facade scene's camera object (Scr2_0487DA): scene end after $12C, four white flashes with big shakes at random gaps (lightning), a last flash, then it rises (Op2_JumpAnim)
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_0487DA: Scene_EndAfterDelay $12C, loop 4 x (Fade_FlashWhite3, Shake_StartBigIfIdle, random wait), Fade_FlashWhite6, JumpAnim $FF98 $780; idle 3335 + 3925n
; ---------------------------------------------------------------------------
Story_SpawnFacadeCamera:
	lea.l	(Scr2_0487DA).l,a2
	jmp	(Obj_SpawnEnemy).l
