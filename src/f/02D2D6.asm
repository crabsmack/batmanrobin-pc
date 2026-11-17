; ---------------------------------------------------------------------------
; Enemy_DomeCraft_SpawnB - wave spawner: the longer green-domed craft course (Scr2_02D2E2, body $280EC): rises from below the screen, holds windows for $2580 then $12C0 ticks, leaves left; fires bursts of 6 shot requests 6 times, then launches 7 spinning mines downward, and loops
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_021D3A +$480/+$488 (x $118, screen y $110); f65000 shows the craft (green dome, vertical bar, thrusters) next to the player; TAS f64940
; ---------------------------------------------------------------------------
Enemy_DomeCraft_SpawnB:
	lea.l	(Scr2_02D2E2).l,a2
	jmp	(Obj_SpawnEnemy).l
