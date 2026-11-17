; ---------------------------------------------------------------------------
; Obj_SpawnScripted - spawns a 4-block object run by VM2_RunObject at d4/d5, linked on the main object list
; In:  d4/d5 = x/y
; Out: d0 = 0 and a6/a3/a4/a5 = the object and its blocks; Z clear when the pool is short
; Evidence: a0 = $7D2 (VM2_RunObject), d7 = 4 then Obj_Spawn; callers Obj_SpawnScriptedOrQuit, $1E58C, $29250
; ---------------------------------------------------------------------------
Obj_SpawnScripted:
	lea.l	($7D2).w,a0
	moveq	#$4,d7
