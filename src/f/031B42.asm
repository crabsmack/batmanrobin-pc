; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnTwoDomes - spawns the airship's dome-cannon mid-boss, first form: the core (Boss_DomeCannon_SpawnCore, child $2A) and two left-facing missile domes (Boss_DomeCannon_SpawnDomeLeft, $2C/$2E at (-$68, -$2E)/(-$68, $2F)); its coroutine 1 schedules the domes' launch scripts and, after $1A40, the core's boss-meter coroutine
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2 +$70C; Scr2_031B4E (Op2_SpawnChildOrWait, Op2_SyncChild, Op2_StartChildCoroutine1Rel), then body $3050A runs the children inline (Op2_RunChild) and kills itself when none is left; TAS f70036 (png f070440: the large dome right of the player, two domes to its left; core destroyed by f71100)
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnTwoDomes:
	lea.l	(Scr2_031B4E).l,a2
	jmp	(Obj_SpawnEnemy).l
