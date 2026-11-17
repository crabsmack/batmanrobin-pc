; ---------------------------------------------------------------------------
; StageCard_SpawnTitle1 - spawns the LEVEL ONE title object: sprite text on palette line 3 that enters from x -136 at y 48 and arcs right to a stop
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A280: ModSprAttr $9860, SetPos ($FF78, $30), ArcX ($FF60, $2D0), then every frame Text_DrawAtObject string $14092 'LEVEL ONE'; called by the director (CallNativeRelSaved) at f802
; ---------------------------------------------------------------------------
StageCard_SpawnTitle1:
	lea.l	(Scr2_04A280).l,a2
	jmp	(Obj_SpawnEnemy).l
