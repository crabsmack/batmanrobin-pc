; ---------------------------------------------------------------------------
; Story_SpawnTowerCamera - spawns the tower scene's object (Scr2_0486DE): starts 340 px up and drops in an arc ($960 ticks), draws a 10-frame animation at +($D9, $4B), then a shake, another animation and Scene_EndAfterDelayWhite
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_0486DE: SetPos 0 $FEAC, JumpAnim $FEAC $960, MovePos $D9 $4B, SetTimer0 $5A0, SetShakeTimer 2, SetAnim, CallNative Scene_EndAfterDelayWhite; idle 3165 + 3925n
; ---------------------------------------------------------------------------
Story_SpawnTowerCamera:
	lea.l	(Scr2_0486DE).l,a2
	jmp	(Obj_SpawnEnemy).l
