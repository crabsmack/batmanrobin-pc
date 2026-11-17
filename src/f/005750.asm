; ---------------------------------------------------------------------------
; Op4_OffsetSpawnXY - a4 op: adds signed pixel offsets to the integer words of the spawn position registers d4/d5 (16.16)
; In:  a4 = script: word dx, word dy; d4/d5 = spawn x/y
; Out: falls into Op4_OffsetSpawnY for dy, then NEXT
; Evidence: swap / add.w (a4)+ / swap on d4; script uses $254E6 ($A,$A), $254FE (-$F,-8) after $63A2 (own position) and before Op4_JitterSpawnXY + Op4_Spawn; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_OffsetSpawnXY:
	swap	d4
	add.w	(a4)+,d4
	swap	d4
; ---------------------------------------------------------------------------
; Op4_OffsetSpawnY - a4 op: adds a signed pixel offset to the integer word of the spawn y register d5 (16.16)
; In:  a4 = script: word dy; d5 = spawn y
; Out: NEXT
; Evidence: label-only entry inside Op4_OffsetSpawnXY (computed-jump target in 5 corpus runs); script use $433E8 (`5756 0014`) before Op4_OffsetSpawnHeading + Op4_Spawn
; ---------------------------------------------------------------------------
Op4_OffsetSpawnY:
	swap	d5
	add.w	(a4)+,d5
	swap	d5
	movea.w	(a4)+,a0
	jmp	(a0)
