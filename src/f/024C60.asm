; ---------------------------------------------------------------------------
; Enemy_GreenThug_SpawnA - lift trigger callback: spawns the green-suited thug without a barrel, the barrel thugs' sibling Scr2_024C6C on the body entry $024CD8 (coroutine 0 $024E6E, hit handler $024D4C whose death path explodes without dropping a barrel), drop action $25, random $F0-$1E0 wait
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $0211D0-$021288 in Tbl_02105A; Op2_SetVector2HereGoto $024CD8; TAS f22330-f26678 (8 spawns); RAM f24840: two objects with coroutine 2 = $024D4C at x 131/191 y 72 = the two green-suited thugs without barrels on the girder in shot f24840
; ---------------------------------------------------------------------------
Enemy_GreenThug_SpawnA:
	lea.l	(Scr2_024C6C).l,a2
	jmp	(Obj_SpawnEnemy).l
