; ---------------------------------------------------------------------------
; Boss_Stage2_FloorTurning - the turning exit of Boss_Stage2_WaitFloorTurn: resumes the boss coroutine at $025D1E, which drops the hull lower (Op2_SetCruiseY $855, Op2_SetAccelYForDist) and then calls Boss_Stage2_EndPlayerTilts
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: bne target in Boss_Stage2_WaitFloorTurn; TAS f33526 (one frame); png f033600: the hull lower over a tilted floor
; ---------------------------------------------------------------------------
Boss_Stage2_FloorTurning:
	jsr	($7FE).w
