; ---------------------------------------------------------------------------
; Op4_OffsetSpawnHeading - a4 op: adds a signed amount to the integer word of the spawn heading register d3 ($400 units per turn), e.g. to fan out a row of spawns
; In:  a4 = script: word dh; d3 = spawn heading (16.16)
; Out: NEXT
; Evidence: swap / add.w (a4)+,d3 / swap; 167 a4-script uses, e.g. $13144-$13174 (`576A 0040 5606 ...` repeated: a fan of objects $40 apart); 52 corpus runs
; ---------------------------------------------------------------------------
Op4_OffsetSpawnHeading:
	swap	d3
	add.w	(a4)+,d3
	swap	d3
	movea.w	(a4)+,a0
	jmp	(a0)
