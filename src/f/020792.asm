; ---------------------------------------------------------------------------
; Stage1_SpawnGiftBox - trigger callback of the stage 1 parade-truck section: spawns a breakable gift box (Scr2_02079E: energy $20, one front-drawn frame) that drops the item in the entry's top byte
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: entries $0F/$15/$27/$1E/$28 << 24 | $020792 in Tbl_0166EA ($0166F6-$01674A; scene $1FECC); TAS f13247-f18552, level-skip f2961; RAM f13800: Scr2_02079E object at x 1271 y 165 = the red present on the truck (shot f13790)
; ---------------------------------------------------------------------------
Stage1_SpawnGiftBox:
	lea.l	(Scr2_02079E).l,a2
	jmp	(Obj_SpawnEnemy).l
