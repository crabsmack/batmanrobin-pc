; ---------------------------------------------------------------------------
; Coll_MoveScene - moves a box through the scene's terrain by calling g_TerrainFn, or g_TerrainFnAlt when the mover's flag bit 0 of (a3) is set
; In:  a3 = motion block, d1/d2 = move dx/dy, d4/d5 = box position, d6/d7 = collision mask words
; Out: from the terrain routine: d4/d5 = new position, d3, d6 = wall result, d7 = floor result (stored to +$50/$52/$54 by the caller)
; Evidence: callers the a2 move op $4CDA (twice, before writing +$50/+$52/+$54 and clearing velocity on contact) and $15730; the routines come from the scene script ($99D4) or Coll_SetFlatFloor
; ---------------------------------------------------------------------------
Coll_MoveScene:
	btst	#$0,(a3)
	bne.s	loc_010DD6
	movea.l	(g_TerrainFn).w,a0
	jmp	(a0)
loc_010DD6:
	movea.l	(g_TerrainFnAlt).w,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Coll_MoveOnBlockMap - moves a box through the scene's plane-A block map: Coll_WalkBlockMap (floor-following) when bit 0 of (a3) is set, else Coll_MoveBlockMap (airborne pixel steps), preserving d1-d2/a0-a6
; In:  as Coll_MoveScene
; Out: d3-d7 from the block-map routine
; Evidence: jsr from $24FAC; the two callees walk the 16x16 block map (mask word + solid top/bottom byte pair per pixel column; NOTES 'Block-map terrain collision', worker 6); executed in 10 fuzz runs (frames 23400-30600). Was Coll_MoveHeightMap: renamed when the callees were understood
; ---------------------------------------------------------------------------
Coll_MoveOnBlockMap:
	btst	#$0,(a3)
	bne.s	Coll_MoveGroundedBlockMap
; ---------------------------------------------------------------------------
; Coll_MoveAirborneBlockMap - Coll_MoveOnBlockMap's airborne path without the bit test: Coll_MoveBlockMap (pixel steps) with d1-d2/a0-a6 preserved
; In:  as Coll_MoveScene
; Out: d0/d3-d7 from Coll_MoveBlockMap
; Evidence: a runtime entry of the reference (a computed jump reached $010DE2, no static reference: a terrain routine pointer in scene data, inferred); movem / bsr.w Coll_MoveBlockMap / movem / rts
; ---------------------------------------------------------------------------
Coll_MoveAirborneBlockMap:
	movem.l	d1-d2/a0-a6,-(a7)
	bsr.w	Coll_MoveBlockMap
	movem.l	(a7)+,d1-d2/a0-a6
	rts
; ---------------------------------------------------------------------------
; Coll_MoveGroundedBlockMap - Coll_MoveOnBlockMap's grounded path without the bit test: Coll_WalkBlockMap (floor-following) with d1-d2/a0-a6 preserved
; In:  as Coll_MoveScene
; Out: d0/d3-d7 from Coll_WalkBlockMap
; Evidence: movem / bsr.w Coll_WalkBlockMap / movem / rts; terrain routine pointer $010DF0 in the scene scripts at $01FBFE, $01FD1A, $01FE2A, $03FE3C, $04004C (paired with $010DE2); entered by the stepped scenario-level-skip at sync 1,188 (cov14)
; ---------------------------------------------------------------------------
Coll_MoveGroundedBlockMap:
	movem.l	d1-d2/a0-a6,-(a7)
	bsr.w	Coll_WalkBlockMap
	movem.l	(a7)+,d1-d2/a0-a6
	rts
; ---------------------------------------------------------------------------
; Coll_NoTerrain - terrain routine for scenes with no ground: applies the move and reports no contact
; In:  d1/d2 = move, d4/d5 = position
; Out: d4/d5 moved, d6 = d7 = 0
; Evidence: terrain routine pair in the scene scripts at $401AA, $4798A, $47B78; executed in 7 runs (fuzz-102600..106200, level-skip)
; ---------------------------------------------------------------------------
Coll_NoTerrain:
	add.w	d1,d4
	add.w	d2,d5
	moveq	#$0,d6
	moveq	#$0,d7
	rts
; ---------------------------------------------------------------------------
; Coll_ScriptSetFlatFloor - native op: Coll_SetFlatFloor with the floor height taken from the next script word
; In:  a2 = operand (word floor y)
; Out: a2 past the word
; Evidence: native callback (a2 op $2CBC) at $1C4FC; executed only in the TAS
; ---------------------------------------------------------------------------
Coll_ScriptSetFlatFloor:
	move.w	(a2)+,d0
