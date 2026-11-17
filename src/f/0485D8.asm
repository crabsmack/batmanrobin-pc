; ---------------------------------------------------------------------------
; Story_SpawnSearchlightCamera - spawns the searchlight scene's object (Scr2_0485E4): scene end after $168, an x arc from -400 (Op2_ArcX), periodic shakes, three sprites drawn at +($10C, $DF); Story_SearchlightScroll scrolls the planes by its x
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_0485E4: Scene_EndAfterDelay $168, SetX/ArcX $FE70 $12C0, SetShakeTimer $14 every $280, PushPos/MovePos $10C $DF/Draw/PopPos; idle 2774 + 3925n
; ---------------------------------------------------------------------------
Story_SpawnSearchlightCamera:
	lea.l	(Scr2_0485E4).l,a2
	jmp	(Obj_SpawnEnemy).l
