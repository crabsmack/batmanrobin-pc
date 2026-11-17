; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn5E - spawns a Freeze-fight hover pod with the drop table $438DC on course 5 ($4479A: the course-4 waypoints from the second) with attack E ($44AAA: rounds of five shots $40 ticks apart), x cruise $1AA, y $155
; In:  d4/d5 = position (Spawn_AtScreenCentre, the Freeze fight script's Op2_CallNative $008D12)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_04446C: gosub $438DC, energy $200/$240, coroutine 0 $4479A, coroutine 1 $44AAA, JitterPos $12C $28, MoveY -$96, Goto $42152; dc.l of Op2_CallNative Spawn_AtScreenCentre at $0470EE, $04712C in the Freeze fight scripts; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn5E:
	lea.l	(Scr2_04446C).l,a2
	jmp	(Obj_SpawnEnemy).l
