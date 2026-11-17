; ---------------------------------------------------------------------------
; Boss_DomeCannon_SpawnCore - spawns the dome-cannon core as an unlinked child: spawn action $2E, then body $300EE: owner $B, energy $1400/$2800 (1P/2P), score $96, a large dome whose barrel turns toward the targeted player; on death BossMeter_Defeated and a weapon-drop stop; coroutine $3015A (started by the parent) shows the boss meter and starts a weapon drop
; In:  d4/d5 = position (the parent's)
; Out: Obj_SpawnEnemyUnlinked's result
; Evidence: dc.l of Op2_SpawnChildOrWait (field $2A) in the three Boss_DomeCannon_Spawn* scripts; frames $40BA-$40E2 rendered: a big purple dome with a turning cannon; RAM: energy 5120 at f70080 falling to 0 by f71100; TAS 70036-75008 (3)
; ---------------------------------------------------------------------------
Boss_DomeCannon_SpawnCore:
	lea.l	(Scr2_03280A).l,a2
	jmp	(Obj_SpawnEnemyUnlinked).l
