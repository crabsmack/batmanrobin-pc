; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn6F - spawns a Freeze-fight hover pod with the drop table $438DC on course 6 ($447BC: the course-4 waypoints from the third) with attack F ($44BFE: long waits $780/$1680 between aimed rounds), x cruise $1AA, y $155
; In:  d4/d5 = position (Spawn_AtScreenCentre, the Freeze fight script's Op2_CallNative $008D12)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_0444B4: gosub $438DC, energy $200/$240, coroutine 0 $447BC, coroutine 1 $44BFE, JitterPos $12C $28, MoveY -$96, Goto $42152; dc.l of Op2_CallNative Spawn_AtScreenCentre at $046FC6, $0470F8, $047136 in the Freeze fight scripts; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn6F:
	lea.l	(Scr2_0444B4).l,a2
	jmp	(Obj_SpawnEnemy).l
