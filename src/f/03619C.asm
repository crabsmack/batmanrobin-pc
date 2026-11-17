; ---------------------------------------------------------------------------
; Enemy_BouncingBomb_Spawn - spawns a bouncing bomb (boss-class script Scr2_0361A8: small hop, 8x8 body box, bounces with a sound and dust, explodes ($3634C) after a random delay (timer 0 = $1E0 + up to $280 ticks); a player's hit bats it back)
; In:  d4/d5 = position
; Out: Obj_SpawnBoss's result (Z set = spawned)
; Evidence: called by Enemy_BombDropper_DropBomb; TAS f79427-f86692; spinning frames $FC90-$FCEA; png f079440 the orange ball under the dropper
; ---------------------------------------------------------------------------
Enemy_BouncingBomb_Spawn:
	lea.l	(Scr2_0361A8).l,a2
	jmp	(Obj_SpawnBoss).l
