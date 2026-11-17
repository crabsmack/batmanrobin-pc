; ---------------------------------------------------------------------------
; Ending_SpawnSnowGlobe - spawns the Freeze-cell ending's snow globe (Scr2_04A658): starts below the screen at (190, 310), jumps up 140 px and then hovers on an oscillator, animating six RAM-bank frames ($FC54-$FC86 with $FC40/$FC4A)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: entered by the narrator script's CallNativeRelSaved $FEAC at $4A79E after the second text block; Scr2_04A658: SetPos ($BE, $136), JumpAnim $8C/$1680, AllocOsc, SetOscX/Y; f16099; png f16200: the globe with the city rising at the bottom
; ---------------------------------------------------------------------------
Ending_SpawnSnowGlobe:
	lea.l	(Scr2_04A658).l,a2
	jmp	(Obj_SpawnEnemy).l
