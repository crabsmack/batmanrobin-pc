; ---------------------------------------------------------------------------
; Flight_SpawnAirshipPathObj - spawns the flying section's airship path object (enemy class): its script is the native op at Tbl_02F600, which runs Flight_AirshipPathObjPark
; In:  none (called by the flying-section scene init $33274)
; Out: Obj_SpawnEnemy's result
; Evidence: jsr from $33274 before the scene's VM6_Run; TAS f35420, level-skip scenario 4 times (each flight-section start)
; ---------------------------------------------------------------------------
Flight_SpawnAirshipPathObj:
	lea.l	(Tbl_02F600).l,a2
	jmp	(Obj_SpawnEnemy).l
