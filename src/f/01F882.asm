; ---------------------------------------------------------------------------
; Enemy_Copter_SpawnRedFast - wave-script spawner of the faster red propeller drone (Scr2_01F898: energy $10, cruise $3C0)
; In:  d4/d5 = x/y, a1 = wave-script operand (word delay)
; Out: as Enemy_Copter_SpawnRed
; Evidence: Scr2_01F898: score $32, SetEnergy $10, cruise $3C0, anim $FD38 (RAM bank frame); Tbl_0174F0 +$37A..; shot f16080 (red drones with rotors over the truck, tagged); 139 sampled TAS objects, f9137-f18692
; ---------------------------------------------------------------------------
Enemy_Copter_SpawnRedFast:
	jsr	(Obj_SpawnScriptedOrQuit).l
	move.l	#$1F898,$2A(a4)
	move.w	(a1)+,$14(a4)
	moveq	#$0,d0
	rts
