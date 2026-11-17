; ---------------------------------------------------------------------------
; Attract_SpawnSkyscraperCamera - spawns the skyscraper scene's camera object (Scr2_048B2A): climbs, then rises faster (Op2_CruiseUp), eases off and ends the scene after $C8, flashing the line tint (Tint_FlashA2 / Tint_FlashE6, lightning) until then
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_048B2A: SetCruiseY $A55, CruiseUp, CallNativeRelSaved Attract_SkyscraperScroll every frame, Scene_EndAfterDelay $C8, Tint_FlashA2 / Tint_FlashE6 loop; TAS f391, idle 391 + 3925n
; ---------------------------------------------------------------------------
Attract_SpawnSkyscraperCamera:
	lea.l	(Scr2_048B2A).l,a2
	jmp	(Obj_SpawnEnemy).l
