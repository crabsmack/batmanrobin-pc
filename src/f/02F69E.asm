; ---------------------------------------------------------------------------
; Flight_SpawnStrobeLight - spawns a white strobe light on the airship hull: attr palette 0, anim $2F764 (four flashes of frames $42D6/$42E0, then dark 15-45 frames: Op2_SetAnimDelayRand $F00, $1E00), body $2F746 (map-anchored, drawn behind, no box)
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: 32 trigger entries in Tbl_0307C2 (pairs at map y $CB/$115 and $77/$16A); frames rendered: a white half-disc glow; TAS 67420-75171 (20 spawns). Raw, never run: an H-flipped twin at $2F6B6 (Scr2_02F6C2)
; ---------------------------------------------------------------------------
Flight_SpawnStrobeLight:
	lea.l	(Scr2_02F6AA).l,a2
	jmp	(Obj_SpawnEnemy).l
