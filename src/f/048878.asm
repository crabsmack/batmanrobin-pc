; ---------------------------------------------------------------------------
; Story_SpawnMonitorsCamera - spawns the monitors scene's object (Scr2_048884): coroutine-2 native first Story_MonitorsScroll, it pans down, bounces, and after $B40 ticks starts a shake and switches to Story_MonitorsAlarmScroll with six random shakes; a coroutine runs the a6 loader scripts that animate the screens
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_048884: StartCoroutine2Rel to $0489CC then $048998 (Op2_CallSlot2Native each frame), CruiseDown, JumpAnim $5C $3C0, Shake_Start/Shake_StartLongIfIdle, Op2_StartA6Script $047F6E/$048394/$0480D0/$047F14/$048232, Scene_EndAfterDelay $78; idle 2066 + 3925n
; ---------------------------------------------------------------------------
Story_SpawnMonitorsCamera:
	lea.l	(Scr2_048884).l,a2
	jmp	(Obj_SpawnEnemy).l
