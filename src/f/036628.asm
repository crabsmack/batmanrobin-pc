; ---------------------------------------------------------------------------
; Enemy_RabbitPot_SpawnE - rabbit pot (Scr2_036634: energy $180, score $12C, body Scr4_03575E) placed at screen (350, 115) that after $1E0 ticks puffs 20 shots $F0 ticks apart, then lets out a bomb rabbit (Spawn_AtOwnOfs) and waits $C0, repeating
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_036634: Enemy_RabbitPot_SpawnA's opening with Op2_PlaceOnScreen $15E $73 (not PlaceYOnScreen), coroutine 1 $0357C4, coroutine 0 $03672C (Op2_FxSpawnOff x 20, Op2_CallNative Spawn_AtOwnOfs, Op4_ChildJitterX), Goto $03575E; the wave spawn entry at $034926 (x $14A, y $E070) names it (the long at $00149E is a coincidence); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_RabbitPot_SpawnE:
	lea.l	(Scr2_036634).l,a2
	jmp	(Obj_SpawnEnemy).l
