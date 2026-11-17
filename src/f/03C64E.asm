; ---------------------------------------------------------------------------
; Boss_ForestRobot_Spawn - spawner of the forest-clearing boss, a giant copper robot with a jowly snouted face and a claw arm (Scr2_03C65A: 5000 points, energy $1200/$1B00, BossMeter_Show, drops in from above at cruise y $A00, swaps its player target every $780 ticks)
; In:  d4/d5 = position (Spawn_AtCameraOfs: camera + (0, -$64))
; Out: jumps to Obj_SpawnBoss with a2 = Scr2_03C65A
; Evidence: dc.l $03C64E after Spawn_AtCameraOfs at the end of the reveal director Scr2_03CB90 ($03CC70); TAS f100733; the robot at screen (38,11) in RAM f101100 and on screen at f101300/f101400 (BOSS TIME)
; ---------------------------------------------------------------------------
Boss_ForestRobot_Spawn:
	lea.l	(Scr2_03C65A).l,a2
	jmp	(Obj_SpawnBoss).l
