; ---------------------------------------------------------------------------
; Stage1_SpawnBalloonLauncherB - second balloon-bomb launcher of the stage 1 parade-truck section: spawns Scr2_020094 at the right screen edge and points it at height table A
; In:  a6 = wave director (wave script Tbl_0174F0)
; Out: d0 = 0, new object's $1E = Tbl_020354 (via Stage1_BalloonUseHeightsA); quits the caller when the pool is full
; Evidence: dc.l at $017DDA in Tbl_0174F0; TAS f16330 (camera x 3190: launcher at x 3515, y 170, balloons Scr2_0203E0 around it at f16380); the script alternates runs of Stage1_LaunchBalloonA and ...B calls
; ---------------------------------------------------------------------------
Stage1_SpawnBalloonLauncherB:
	lea.l	(Scr2_020094).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Stage1_BalloonUseHeightsA).l
