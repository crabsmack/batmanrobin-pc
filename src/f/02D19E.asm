; ---------------------------------------------------------------------------
; Enemy_DomeCraft_SpawnA - wave spawner: a green-domed craft (Scr2_02D1AA, body $280EC) that rises from below the screen and holds a sequence of player-relative windows, then cruises off left; fires bursts of 3 shot requests ($28156) 6 times, then launches 2 x 6 spinning mines downward ($2999E)
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_021D3A +$2EC/+$310 (x $118, screen y $110, drop actions 0/$2D); TAS f41668
; ---------------------------------------------------------------------------
Enemy_DomeCraft_SpawnA:
	lea.l	(Scr2_02D1AA).l,a2
	jmp	(Obj_SpawnEnemy).l
