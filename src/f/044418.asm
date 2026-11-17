; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn4D - spawns a Freeze-fight hover pod with the drop table $438DC on course 4 ($44778: waypoint gosubs $45352, $4535E, $45342, $7BC0 ticks each) with attack D ($44962: turn rate $200, shots $A0 ticks apart), energy $200/$240, x/y cruise $155
; In:  d4/d5 = position (Spawn_AtScreenCentre, the Freeze fight script's Op2_CallNative $008D12)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_044424: gosub $438DC (drop: Op2_BranchWeaponLvlMaxGE, SwitchRandom, SetSpawnAction), coroutine 0 $44778, coroutine 1 $44962, JitterPos $12C $28, MoveY -$96, Goto the hover pod body $42152; dc.l of Op2_CallNative Spawn_AtScreenCentre at $046FB6, $047046, $047056, $0470E4, $047122 in the Freeze fight scripts; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn4D:
	lea.l	(Scr2_044424).l,a2
	jmp	(Obj_SpawnEnemy).l
