; ---------------------------------------------------------------------------
; Boss_BeamShip_Spawn - spawns the stage 2 flight section's final boss: an unlinked radius object (script Scr2_029598) and the ship itself (script $2DF94), linked to each other ($22 of the ship = the radius object, $1E of the radius object = the ship)
; In:  d4/d5 = x/y (the scene sets $172, $70)
; Out: d0 = 0, or -1 when the pool is short (the radius object is freed again)
; Evidence: called by the scene init $33A62 (which also installs the beam/disc frame callback $33A88 and scroll mode $38); TAS f77184; f078000 shows the white ship and its translucent beam ending in a disc round the player
; ---------------------------------------------------------------------------
Boss_BeamShip_Spawn:
	jsr	(Obj_SpawnScriptedUnlinked).l
	bne.s	loc_029286
	move.l	#$29598,$2A(a4)
	movea.l	a6,a1
	jsr	(Obj_SpawnScripted).l
	bne.s	loc_02927E
	move.l	#$2DF94,$2A(a4)
	move.l	a1,$22(a6)
	move.l	a6,$1E(a1)
	moveq	#$0,d0
	rts
loc_02927E:
	movea.l	a1,a6
	jsr	(Obj_FreeWithBlocks).l
loc_029286:
	moveq	#-$1,d0
	rts
