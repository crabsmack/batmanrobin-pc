; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn2A - spawns a hall hover pod on course 2 ($4487A: alternating waypoint subroutines at a fixed cruise) with attack A ($44D68, rounds of six aimed shots), x speed $200
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_044544: as Enemy_HoverPod_Spawn1A with x accel/cruise $200 (distance $60) and coroutine 0 $4487A; wave entries Tbl_041138 +$D6, +$17E, +$1F4; TAS 117398-118422 (3 spawns)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn2A:
	lea.l	(Scr2_044544).l,a2
	jmp	(Obj_SpawnEnemy).l
