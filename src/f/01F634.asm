; ---------------------------------------------------------------------------
; Enemy_Copter_SpawnGreenFast - wave-script spawner of the green propeller drone (Scr2_01F64A: energy $40, counts 2 in the population, cruise $3C0), delay word as Enemy_Copter_SpawnRed
; In:  d4/d5 = x/y, a1 = wave-script operand (word delay)
; Out: as Enemy_Copter_SpawnRed
; Evidence: Scr2_01F64A: AddPopulation 2, SetScoreValue $64, SetEnergy $40, cruise/accel $3C0, anim $386; Tbl_0174F0 +$384..; shot f9360 (green drone at the Gem Expo sign, tagged); TAS f9167-f15975
; ---------------------------------------------------------------------------
Enemy_Copter_SpawnGreenFast:
	jsr	(Obj_SpawnScriptedOrQuit).l
	move.l	#$1F64A,$2A(a4)
	move.w	(a1)+,$14(a4)
	moveq	#$0,d0
	rts
