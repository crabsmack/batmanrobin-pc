; ---------------------------------------------------------------------------
; Enemy_Copter_SpawnGreen - wave-script spawner of the slower green propeller drone (Scr2_01F772: energy $40, cruise $2EA)
; In:  d4/d5 = x/y, a1 = wave-script operand (word delay)
; Out: as Enemy_Copter_SpawnRed
; Evidence: Scr2_01F772: IncPopulation, score $64, SetEnergy $40, cruise $2EA, anim $260; Tbl_0174F0 +$8A2/+$8C0; shot f16080 (green drone at the right edge, tagged); TAS f16049-f18692 (3 frames)
; ---------------------------------------------------------------------------
Enemy_Copter_SpawnGreen:
	jsr	(Obj_SpawnScriptedOrQuit).l
	move.l	#$1F772,$2A(a4)
	move.w	(a1)+,$14(a4)
	moveq	#$0,d0
	rts
