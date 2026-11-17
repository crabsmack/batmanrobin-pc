; ---------------------------------------------------------------------------
; Coll_SetFlatFloor - makes the scene terrain a flat floor at height d0: installs Coll_FlatFloor / Coll_FlatFloorRest as the terrain routines
; In:  d0 = floor y
; Out: g_FloorY, g_TerrainFn = $10E20, g_TerrainFnAlt = $10E46
; Evidence: called from $1EE24 and through Coll_ScriptSetFlatFloor; ran on TAS frames 10670 and 18878
; ---------------------------------------------------------------------------
Coll_SetFlatFloor:
	move.w	d0,(g_FloorY).w
	move.l	#$10E20,(g_TerrainFn).w
	move.l	#$10E46,(g_TerrainFnAlt).w
	rts
; ---------------------------------------------------------------------------
; Coll_FlatFloor - terrain routine for a flat floor at g_FloorY: applies the move and lands (d7 = 1, y = floor) only when the move crosses from at or above the floor to below it and mask bit 0 is set
; In:  d1/d2 = move, d4/d5 = position, d7 = mask (bit 0 floor)
; Out: d4/d5, d3 = d6 = 0, d7 = 1 landed else 0
; Evidence: installed as g_TerrainFn by Coll_SetFlatFloor and the scene scripts ($32E3E, $3FF5A, $4039E, $47570, $4771A); executed in 40 corpus runs
; ---------------------------------------------------------------------------
Coll_FlatFloor:
	move.w	(g_FloorY).w,d0
	add.w	d1,d4
	moveq	#$0,d3
	moveq	#$0,d6
	btst	#$0,d7
	beq.s	loc_010E40
	cmp.w	d0,d5
	bgt.s	loc_010E40
	add.w	d2,d5
	cmp.w	d0,d5
	ble.s	loc_010E42
	move.w	d0,d5
	moveq	#$1,d7
	rts
loc_010E40:
	add.w	d2,d5
loc_010E42:
	moveq	#$0,d7
	rts
; ---------------------------------------------------------------------------
; Coll_FlatFloorRest - as Coll_FlatFloor but also reports contact while resting on the floor (lands when the new y >= floor - 1)
; In:  as Coll_FlatFloor
; Out: as Coll_FlatFloor
; Evidence: installed as g_TerrainFnAlt beside Coll_FlatFloor (movers with (a3) bit 0 set); executed in the same 40 runs
; ---------------------------------------------------------------------------
Coll_FlatFloorRest:
	move.w	(g_FloorY).w,d0
	add.w	d1,d4
	moveq	#$0,d3
	moveq	#$0,d6
	btst	#$0,d7
	beq.s	loc_010E6C
	cmp.w	d0,d5
	bgt.s	loc_010E6C
	subq.w	#$1,d0
	add.w	d2,d5
	cmp.w	d0,d5
	blt.s	loc_010E6E
	addq.w	#$1,d0
	move.w	d0,d5
	move.w	#$1,d7
	rts
loc_010E6C:
	add.w	d2,d5
loc_010E6E:
	moveq	#$0,d7
	rts
; ---------------------------------------------------------------------------
; Coll_Stage1StreetFloors - terrain routine of the stage 1 opening street: the street at y $C8 (mask bit 0), and above it a walkway at y $6C (bit 2) or $60 (bit 1) present where (x * 1.09375) mod 360 is in $10-$150
; In:  d1/d2 = move, d4/d5 = position, d7 = mask
; Out: d4/d5, d3 = d6 = 0, d7 = 1 street / 2 or 4 walkway / 0 none
; Evidence: terrain routine pair in the scene table at $1FAA6; TAS RAM frame 3000: g_TerrainFn = g_TerrainFnAlt = $10E72; executed in fuzz-01800..05400, level-skip, robin and two-player (robin frame 1800: the two-level street with the upper walkway)
; ---------------------------------------------------------------------------
Coll_Stage1StreetFloors:
	add.w	d1,d4
	cmpi.w	#$6C,d5
	bgt.s	loc_010ED8
	moveq	#$0,d0
	move.w	d4,d0
	move.w	d0,d3
	lsr.w	#$3,d3
	add.w	d3,d0
	lsr.w	#$2,d3
	sub.w	d3,d0
	divu.w	#$168,d0
	swap	d0
	cmpi.w	#$150,d0
	bgt.s	loc_010EFA
	cmpi.w	#$10,d0
	blt.s	loc_010EFA
	btst	#$1,d7
	beq.s	loc_010EA6
	cmpi.w	#$60,d5
	ble.s	loc_010EC2
loc_010EA6:
	btst	#$2,d7
	beq.s	loc_010EFA
	add.w	d2,d5
	cmpi.w	#$6C,d5
	blt.s	loc_010EFC
	move.w	#$6C,d5
	moveq	#$0,d3
	moveq	#$0,d6
	move.w	#$4,d7
	rts
loc_010EC2:
	add.w	d2,d5
	cmpi.w	#$60,d5
	blt.s	loc_010EFC
	move.w	#$60,d5
	moveq	#$0,d3
	moveq	#$0,d6
	move.w	#$2,d7
	rts
loc_010ED8:
	btst	#$0,d7
	beq.s	loc_010EFA
	cmpi.w	#$C8,d5
	bgt.s	loc_010EFA
	add.w	d2,d5
	cmpi.w	#$C8,d5
	blt.s	loc_010EFC
	move.w	#$C8,d5
	moveq	#$0,d3
	moveq	#$0,d6
	move.w	#$1,d7
	rts
loc_010EFA:
	add.w	d2,d5
loc_010EFC:
	moveq	#$0,d3
	moveq	#$0,d6
	moveq	#$0,d7
	rts
; ---------------------------------------------------------------------------
; Coll_HatterArenaFloor - terrain routine of the Mad Hatter boss arena: a floor at y $D4 only for x in $94-$206 (mask bit 0), elsewhere the mover falls
; In:  d1/d2 = move, d4/d5 = position, d7 = mask
; Out: d4/d5, d3 = d6 = 0, d7 = 1 landed else 0
; Evidence: terrain routine pair in the scene script at $40514; executed in the TAS, fuzz-113400/115200 and level-skip; TAS frames 114000-115200 show the checkerboard Mad Hatter arena
; ---------------------------------------------------------------------------
Coll_HatterArenaFloor:
	moveq	#$0,d3
	moveq	#$0,d6
	add.w	d1,d4
	cmpi.w	#$94,d4
	blt.s	loc_010F32
	cmpi.w	#$206,d4
	bgt.s	loc_010F32
	btst	#$0,d7
	beq.s	loc_010F32
	cmpi.w	#$D4,d5
	bgt.s	loc_010F32
	add.w	d2,d5
	cmpi.w	#$D4,d5
	blt.s	loc_010F34
	move.w	#$D4,d5
	moveq	#$1,d7
	rts
loc_010F32:
	add.w	d2,d5
loc_010F34:
	moveq	#$0,d7
	rts
