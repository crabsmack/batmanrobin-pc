; ---------------------------------------------------------------------------
; Stage3_SpawnRevealDirector - loader-script native that spawns the forest-clearing reveal director (boss class, Scr2_03CB90): boss music $A, the line-3 colour cycle, Stage3_RevealInit, 36 timed Stage3_RevealBurst bursts, flashes and shakes while Stage3_RevealNextCell fills the rest, then Boss_ForestRobot_Spawn
; In:  -
; Out: jumps to Obj_SpawnBoss with a2 = Scr2_03CB90 (d0 = 0 spawned; nonzero makes Op6_CallNativeRetry call it again next frame)
; Evidence: `70BE 0003CB84` (Op6_CallNativeRetry) at Scr6_040054+$88, the clearing scene's loader; TAS f97887 (once); the picture assembling at f98400-f100800
; ---------------------------------------------------------------------------
Stage3_SpawnRevealDirector:
	lea.l	(Scr2_03CB90).l,a2
	jmp	(Obj_SpawnBoss).l
