; ---------------------------------------------------------------------------
; Stage1_SpawnBalloonLauncherA - wave-script spawner of the stage 1 parade-truck section: spawns the balloon-bomb launcher Scr2_01FFB8 (an unseen emitter at the right screen edge) and points it at height table B
; In:  a6 = wave director (a5-VM wave script Tbl_0174F0 calls it)
; Out: d0 = 0, new object's $1E = Tbl_02036E (via Stage1_BalloonUseHeightsB); on a full pool Obj_SpawnEnemyOrQuit quits the caller
; Evidence: dc.l at $017C3C in the wave script Tbl_0174F0; TAS f13756 (scene $1FECC, the truck section): the object sits at screen x 325, y 162 while balloons rise from the car on the right (shot f13790); its script calls Stage1_LaunchBalloonB/A
; ---------------------------------------------------------------------------
Stage1_SpawnBalloonLauncherA:
	lea.l	(Scr2_01FFB8).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Stage1_BalloonUseHeightsB).l
