; ---------------------------------------------------------------------------
; ToyChain_MakeDropIn12B - inline chain maker of a drop-in chain: 12 figures (table $039BF0) on the drop-in chain body $037B8A, as ToyChain_MakeDropIn12 with its own link table
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $039B5C (script $039B5C, Goto the drop-in base body $037B12); its only namer is the script of the raw stub $039B50 (lea script / jmp Obj_SpawnEnemy), which no ROM long, table or op operand names: no reachable caller is known; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeDropIn12B:
	jsr	ToyChain_SpawnInline(pc)
