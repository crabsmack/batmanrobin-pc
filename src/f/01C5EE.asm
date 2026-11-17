; ---------------------------------------------------------------------------
; Boss_JokerBalloon_LandSound - native fragment of the balloon boss script: after a floor touch (dust effect, Shake_StartIfIdle) plays sound $B unless the screen was already shaking, then resumes the script at $1C602
; In:  a2 script context (entered by Op2_Native $7FC at $1C5EC)
; Out: jsr $7FE.w (back to the a2 VM)
; Evidence: script before it: BranchNotFloorMask 3, FxSpawnOff $1E/$1BEA, CallNative Shake_StartIfIdle; tst.l g_ShakeY; Snd_PlaySfx = the sound call of Op2_PlaySound; TAS f19339-f20434 (25 frames)
; ---------------------------------------------------------------------------
Boss_JokerBalloon_LandSound:
	tst.l	(g_ShakeY).w
	bne.s	loc_01C5FE
	move.w	#$B,d0
	jsr	(Snd_PlaySfx).l
loc_01C5FE:
	jsr	($7FE).w
