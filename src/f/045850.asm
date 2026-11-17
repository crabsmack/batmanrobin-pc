; ---------------------------------------------------------------------------
; Enemy_VentTank_SpawnFaster - spawns a lab vent tank (body $42D74, floor height $9E, energy $300) that cruises at $155 and repeats three vent rounds ($780 ticks) then a rest
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_04585C: score $C8, energy $300, x accel/cruise $155, coroutine 0 $4587E (gosub $41D50 chase, waits $780, counter 3), Goto the vent tank body $42D74; the lab wave's spawn entries at $0416B4 and $0416F0 (x $154 / $A0, y $E070) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_VentTank_SpawnFaster:
	lea.l	(Scr2_04585C).l,a2
	jmp	(Obj_SpawnEnemy).l
