; ---------------------------------------------------------------------------
; Attract_SpawnRooftopHeroes - spawns the rooftop scene's object (Scr2_048CE2): two animated sprite blocks (Batman and Robin, inferred from the picture; one blinks/idles on a long delay), Attract_RooftopLightning every frame, lightning flashes (Tint_FlashA2 with sprite var 0 = $20) and Scene_EndAfterDelay $14 at the end
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_048CE2: AllocSprBlock $10, CallNative $048E80, anim with SetAnimDelay $5A00 holds, Tint_FlashA2 + SetSprVar0 $20; idle 926 + 3925n
; ---------------------------------------------------------------------------
Attract_SpawnRooftopHeroes:
	lea.l	(Scr2_048CE2).l,a2
	jmp	(Obj_SpawnEnemy).l
