; ---------------------------------------------------------------------------
; Stage1_SpawnSectionExit - wave-script spawner: spawns the section exit Scr2_0204D4, which shows the GO! sign (keeping the camera lock) and fades the scene out when a player touches its box
; In:  a6 = wave director (wave script Tbl_0174F0)
; Out: d0 from Obj_SpawnEnemy (0 = spawned)
; Evidence: dc.l at $017862 in Tbl_0174F0; script: Op2_CallNative HUD_StartGoSignKeepLock, Op2_BranchTouchBox (-15,-25,15,25) loop, Op2_CallNative Scene_FadeOutExit, Op2_Kill; TAS f9596, 19 frames before the next scene $1FD76 (f9615)
; ---------------------------------------------------------------------------
Stage1_SpawnSectionExit:
	lea.l	(Scr2_0204D4).l,a2
	jmp	(Obj_SpawnEnemy).l
