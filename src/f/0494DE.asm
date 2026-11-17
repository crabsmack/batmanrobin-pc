; ---------------------------------------------------------------------------
; GameOver_SpawnDirector - spawns the game-over director Scr2_0494EA (an a2 script despite its label): ends the scene after 1000 frames, flickers the fade level at random (Fade_FlickerTarget), spawns snowflake effects at the screen centre and scrolls the city (GameOver_ScrollCity) every frame
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_0494EA: Op2_CallNative Scene_EndAfterDelay 0/$3E8, coroutines $4950C (Fade_FlickerTarget loop) and $4951E (Op2_FxPosScreenCentre + Op2_FxSpawnRegs x2), Op2_CallNativeRel $100 -> $49606; scenario-game-over f42833
; ---------------------------------------------------------------------------
GameOver_SpawnDirector:
	lea.l	(Scr2_0494EA).l,a2
	jmp	(Obj_SpawnEnemy).l
