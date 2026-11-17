; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnBossPairJetsFar - spawns a gas orb during the Mr. Freeze fight (jittered, 150 px higher) with the two-jet / single-jet sweeps of the faster-puff set ($43E1C / $43E66, cruise y $1400)
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_043C80: as Enemy_GasOrb_SpawnBossPairJets's Scr2_043AD8 (drop init $438DC, body $41E4A) with coroutine 1 $43D18 picking vector 1 = $43E1C or $43E66; spawner long at $046F46, $047098 in the Freeze fight scripts; reached by scenario-sec-4401-freeze (reference corpus, 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnBossPairJetsFar:
	lea.l	(Scr2_043C80).l,a2
	jmp	(Obj_SpawnEnemy).l
