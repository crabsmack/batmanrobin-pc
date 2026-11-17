; ---------------------------------------------------------------------------
; Title_SpawnMenu - spawns the title director Scr2_0496D8: 500-frame idle timeout, then waits for Start, which opens the START / OPTIONS menu; START ends the scene with result 1, OPTIONS runs the options page until Start
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_0496D8: Scene_EndAfterDelay 0/$1F4, coroutine $49708 (Op2_BranchVar0Any $80 -> $49720), $49720 calls Title_DimPicture, Title_MenuInit, Title_MenuStep; Op2_BranchVar1Eq 1 -> options ($49756); TAS f404
; ---------------------------------------------------------------------------
Title_SpawnMenu:
	lea.l	(Scr2_0496D8).l,a2
	jmp	(Obj_SpawnEnemy).l
