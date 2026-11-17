; ---------------------------------------------------------------------------
; Flight_AirshipPathObjPark - native script start of the path object: stores its a6 in g_SceneVars+$92 and re-enters the a2 VM on Scr2_02F60A, an endless yield, until Flight_AirshipStartPath gives it its path
; In:  a6 = the new path object
; Out: re-enters the script (jsr $7FE)
; Evidence: Op2_Native word $07FC at $2F600 (Tbl_02F600) runs straight into it; RAM: g_SceneVars+$92 ($F586) = $AE62 at TAS f67740, whose pc is $2F66A from f67320; TAS f35433
; ---------------------------------------------------------------------------
Flight_AirshipPathObjPark:
	move.w	a6,(g_SceneVars+$92).w
	jsr	($7FE).w
