; ---------------------------------------------------------------------------
; Enemy_VentTank_SpawnFast - spawns a vent tank for the lab's last arena (floor height $9E) that cruises at $D5 and alternates four vent patterns ($45B8C-$45BE0) with $5A0 / $F0 tick waits
; In:  d4/d5 = x/y (wave entry: x $15E or -$1E)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045A0E: score $C8, energy $300, coroutine 0 $45A26 (x accel/cruise $D5, chase gosub, vent gosubs +$100/+$148/+$126/+$134/+$FA/+$120), body $42D74; wave entries $41AE8/$41AF0 in the arena wave $41A62 (Op5_LockCameraHere); TAS 121763-122237 (3 spawns)
; ---------------------------------------------------------------------------
Enemy_VentTank_SpawnFast:
	lea.l	(Scr2_045A0E).l,a2
	jmp	(Obj_SpawnEnemy).l
