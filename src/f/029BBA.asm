; ---------------------------------------------------------------------------
; Enemy_Jet_SpawnTrackY - wave spawner: a rocket jet (Scr2_029BC6, body $285DE: energy $B0, fires every $40 ticks) carrying a random pickup, that cruises left for one of four random times while following the player's height
; In:  d4/d5 = x/y (from the wave entry)
; Out: as Obj_SpawnEnemy
; Evidence: 18 entries in the wave script Scr5_022C7E (x $154, right of the screen); the script gosubs $29B76 (Op2_SwitchRandom over spawn actions $2D/$17/$1A/$E/$8/$B or none) and its course uses vector 2 = $27D1A (Op2_CruiseToPlayerY until timer 2); frames $3B38-$3B4C rendered: a silver rocket jet with a blue flame; TAS 38691-50838
; ---------------------------------------------------------------------------
Enemy_Jet_SpawnTrackY:
	lea.l	(Scr2_029BC6).l,a2
	jmp	(Obj_SpawnEnemy).l
