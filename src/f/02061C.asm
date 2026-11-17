; ---------------------------------------------------------------------------
; Stage1_SpawnUrn - trigger callback of the stage 1 Gem Expo section: spawns a breakable urn on a pedestal (Scr2_020628: energy $20, one frame, bursts into an effect and gives its score to the hitter)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned (the entry's top byte = its drop)
; Evidence: dc.l entries in the section-2 trigger table Tbl_0163BA ($0163D4 ...; scene $1FC50); TAS f6399-f8887; RAM f6420: Scr2_020628 object at x 378 y 173 (camera 104) = the urn at screen x 274 in shot f6420
; ---------------------------------------------------------------------------
Stage1_SpawnUrn:
	lea.l	(Scr2_020628).l,a2
	jmp	(Obj_SpawnEnemy).l
