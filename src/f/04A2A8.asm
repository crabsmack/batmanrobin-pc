; ---------------------------------------------------------------------------
; StageCard_SpawnSubtitle1 - spawns the HAPPY BIRTHDAY TO ME! subtitle object: sprite text that enters from the right (x 320, y 210) and arcs left to a stop
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A2B4: SetPos ($140, $D2), ArcX ($13B, $3C0), Text_DrawAtObject string $1409C; f832; png f840 shows HAPPY at the right edge
; ---------------------------------------------------------------------------
StageCard_SpawnSubtitle1:
	lea.l	(Scr2_04A2B4).l,a2
	jmp	(Obj_SpawnEnemy).l
