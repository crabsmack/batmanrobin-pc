; ---------------------------------------------------------------------------
; Enemy_Brute_Spawn - spawns the big sack-carrying brute that walks in on the ground: palette line 3, energy $D0, attack loop entered at $1B0CA
; In:  d4/d5 = x/y (trigger or wave entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01AD2A: same prologue as Enemy_Brute_SpawnFromSign but coroutine 0 = $1B032 (GosubVector2Plus 0, anim $382, floor check); referenced from Tbl_0166EA (+$4E8, +$662); TAS f2118-f2422; brute body Scr2_01AF1A drawn as the purple strongman with a sack (shots f2940, f3000)
; ---------------------------------------------------------------------------
Enemy_Brute_Spawn:
	lea.l	(Scr2_01AD2A).l,a2
	jmp	(Obj_SpawnEnemy).l
