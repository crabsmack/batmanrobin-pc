; ---------------------------------------------------------------------------
; Boss_Harley_StopStreetFx - native fragment at the head of the Harley boss script: turns off the stage 1 street's plane-A fire/sky line effects, then resumes the script
; In:  a2 = script after the fragment ($18076)
; Out: g_Stage1FireState = $FF (Stage1_UpdateStreetScroll and Stage1_VIntFacade then leave plane A alone)
; Evidence: Scr2_018066 = 437A 0001 07FC (native) then move.b #$FF,$F518 / jsr $7FE; TAS f4519, right after Boss_Harley_Spawn
; ---------------------------------------------------------------------------
Boss_Harley_StopStreetFx:
	move.b	#$FF,(g_Stage1FireState).w
	jsr	($7FE).w
