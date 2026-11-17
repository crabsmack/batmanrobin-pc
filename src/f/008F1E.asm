; ---------------------------------------------------------------------------
; Obj_SpawnScriptedUnlinked - spawns a 4-block VM2_RunObject object at d4/d5 without linking it on the main object list
; In:  d4/d5 = x/y
; Out: as Obj_SpawnUnlinked
; Evidence: caller $29250 (TAS f77184 only)
; ---------------------------------------------------------------------------
Obj_SpawnScriptedUnlinked:
	lea.l	($7D2).w,a0
	moveq	#$4,d7
