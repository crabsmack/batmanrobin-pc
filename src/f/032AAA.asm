; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnDomeLeft - spawns one of the dome cannon's left-facing missile domes as an unlinked child: spawn action $2D, speed $E6, the shared dome body $2FC56 (H-flipped, energy $150/$300); the parent starts its launch script (coroutine 1) from $32ADA-$32D46
; In:  d4/d5 = position (the parent's)
; Out: Obj_SpawnEnemyUnlinked's result
; Evidence: dc.l of Op2_SpawnChildOrWait (fields $2C/$2E) in Scr2_031B4E, Scr2_031D3C, Scr2_0320EA; Op2_StartChildCoroutine1Rel at $31CC0-$31D24 point into Scr2_032ACE; RAM f70080: two dome bodies (pc $2FC86) beside the core; TAS 70036-75008
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnDomeLeft:
	lea.l	(Scr2_032AB6).l,a2
	jmp	(Obj_SpawnEnemyUnlinked).l
