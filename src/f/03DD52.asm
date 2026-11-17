; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdSpawn0 - road schedule spawn kind 0: a1 = Boss_Hatter_SpawnRoadHazard, then the common spawn code; the next seven 8-byte stubs (raw, never run) pick the kind 1-7 spawners $3FB24, $3FBC2, $3FD9C, $3FDC2, $3FC60, $3FCFE, $3FAF8
; In:  a0/d4/d1 as Boss_Hatter_RoadCmdTbl
; Out: falls into Boss_Hatter_RoadCmdSpawnInLane
; Evidence: lea $3FB0E,a1 / bra.s $3DD90; raw $03DD5A-$03DD8F = seven lea/bra.s pairs; TAS f114753-f114833 (2 frames)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdSpawn0:
	lea.l	(Boss_Hatter_SpawnRoadHazard).l,a1
	bra.s	Boss_Hatter_RoadCmdSpawnInLane
