; ---------------------------------------------------------------------------
; Enemy_BombRabbit_SetBlastGap - native at a bomb rabbit's explosion (after the blast effect $3634C): sets g_ScriptTimer = 20 frames, which holds every other bomb rabbit's explosion until it runs out, then the script kills the rabbit
; In:  -
; Out: g_ScriptTimer = $14; re-enters the a2 VM ($7FE)
; Evidence: a2 native word at $35E8A in Scr2_035D74 (`518C 04C4` FxSpawnHere, then $35E8C, then Op2_Kill $35E96); read by Enemy_BombRabbit_BlastRecent; TAS f82709-f111908 (4 explosions)
; ---------------------------------------------------------------------------
Enemy_BombRabbit_SetBlastGap:
	move.w	#$14,(g_ScriptTimer).w
	jsr	($7FE).w
