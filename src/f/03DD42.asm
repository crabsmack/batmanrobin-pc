; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdTbl_Spawn0 - entry 8 of Boss_Hatter_RoadCmdTbl (spawn kind 0, the road hazard): bra.s to Boss_Hatter_RoadCmdSpawn0; entries 9-15 (kinds 1-7) follow, never run
; In:  as Boss_Hatter_RoadCmdTbl
; Out: -
; Evidence: $03DD32 + 8 x 2; ctarget in 3 corpus runs (TAS f114753/f114833)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdTbl_Spawn0:
	bra.s	Boss_Hatter_RoadCmdSpawn0
