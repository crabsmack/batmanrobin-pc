; ---------------------------------------------------------------------------
; Attract_SpawnPlaneBDriver - spawns the skyscraper scene's second object (Scr2_048B9A): enters at screen (30, 110) with a hover oscillation, drifts, then cruises left; it draws two sprites and Attract_PlaneBFollowObj makes plane B follow it; at the end it starts an a6 loader script and dies
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_048B9A: PlaceOnScreen $1E $6E, AllocOsc/SetOscY, CruiseLeft, CallNativeRelSaved $048CA6 each frame, Op2_StartA6Script $048A62, Kill; TAS f391, idle 391 + 3925n; what plane B shows here is not identified
; ---------------------------------------------------------------------------
Attract_SpawnPlaneBDriver:
	lea.l	(Scr2_048B9A).l,a2
	jmp	(Obj_SpawnEnemy).l
