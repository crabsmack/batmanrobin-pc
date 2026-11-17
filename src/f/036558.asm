; ---------------------------------------------------------------------------
; Enemy_PuffPlant_SpawnG - puff plant (Scr2_036564) whose loop is picked at random (Op2_SwitchRandom): mixes of the three puff scripts ($356EE/$35714/$3573A) in runs of 4-25 puffs; raw never-run spawners for the other variants follow at $0365D6 and $0365FC
; In:  d4/d5 = position
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entries $03407E-$0340D2 (Tbl_033F30); TAS f96435-f96681 (6 spawns)
; ---------------------------------------------------------------------------
Enemy_PuffPlant_SpawnG:
	lea.l	(Scr2_036564).l,a2
	jmp	(Obj_SpawnEnemy).l
