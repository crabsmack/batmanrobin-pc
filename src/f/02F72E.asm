; ---------------------------------------------------------------------------
; Flight_SpawnSearchlight - spawns a searchlight on the airship hull: anim $2F7D8 sweeps a lamp's beam through frames $4312-$433A with offset and flip changes, body $2F746
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: 6 trigger entries in Tbl_0307C2 at map y $F0; frames rendered: a lamp with a red-yellow beam turning from up to sideways; TAS 67496-75351 (6)
; ---------------------------------------------------------------------------
Flight_SpawnSearchlight:
	lea.l	(Scr2_02F73A).l,a2
	jmp	(Obj_SpawnEnemy).l
