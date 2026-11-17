; ---------------------------------------------------------------------------
; ToyChain_MakeChild10B - inline chain maker of a toy-chain base: a 10-figure chain (table $039AA8) at speed $100 with sway cruise $100 on the chain body Scr2_0379DA
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $039686 (script $039686); its only namer is the script of the raw stub $03967A (lea script / jmp Obj_SpawnEnemy), which no ROM long, table or op operand names: no reachable caller is known; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeChild10B:
	jsr	ToyChain_SpawnInline(pc)
