; ---------------------------------------------------------------------------
; Enemy_LabTech_Spawn - spawns a lab technician of the stage 4 lab (white coat, red goggles) at floor height $A2 with a random speed; coroutine 0 cycles its behaviours (hold a range beside the targeted player, swap targets) through vector 0
; In:  d4/d5 = x/y (wave entry: x -$14/-$50 left or $154/$190 right, y $70)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04580E: Op2_SetSpeedRand $26/$E6, coroutine 0 $45820 (Op2_SetVector0HereGoto list into $42AC8-$42B3A, Op2_SwapPlayerTarget), Op2_PlaceYOnScreen $A2, body $42A34 (population +1, score $64, energy $50 1P/2P, Op2_HoldRangeOfPlayerX $50-$5A); 15 wave entries in Scr2_0417CC/Scr5_0413AC (some with spawn action $29); RAM f119460: two objects of this body at x 291/334, the goggled men in white lab coats of shot w29_c119460; TAS 119416-122363 (11 spawns)
; ---------------------------------------------------------------------------
Enemy_LabTech_Spawn:
	lea.l	(Scr2_04580E).l,a2
	jmp	(Obj_SpawnEnemy).l
