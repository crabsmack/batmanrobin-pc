; ---------------------------------------------------------------------------
; Enemy_BombRabbit_SpawnHopFar - spawner of a faster bomb rabbit (script Scr2_035D74: cruise $280, a higher jump $78/$1E0, and a wide touch box of about +-100 px that lights its fuse when a player comes near); its script holds the shared body $035DA2
; In:  d4/d5 = spawn position
; Out: Obj_SpawnEnemy's result
; Evidence: the rabbit pot's Spawn_AtOwnOfs entries ($0367FA, $03680E, $036834 ...) and $038938/$038948; TAS f111825
; ---------------------------------------------------------------------------
Enemy_BombRabbit_SpawnHopFar:
	lea.l	(Scr2_035D74).l,a2
	jmp	(Obj_SpawnEnemy).l
