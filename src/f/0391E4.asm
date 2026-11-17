; ---------------------------------------------------------------------------
; ToyRing_SpawnSix - spawner (5 blocks) of a six-figure toy ring (radius $32, stiffness $60), energy $C0, drop action $25, splitting into single figures later (coroutine 1 $039612)
; In:  d4/d5 = position (trigger entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_0391F4, d7 = 5
; Evidence: ToyRing_Init words 6 $32 $60 1 $9248; Tbl_034142 (+$3B0, +$41E, +$754); TAS f109008-f112758 (4 frames)
; ---------------------------------------------------------------------------
ToyRing_SpawnSix:
	lea.l	(Scr2_0391F4).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
