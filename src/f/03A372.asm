; ---------------------------------------------------------------------------
; ToyChain_MakeBox10D - inline chain maker of a gift box's 10-figure chain (table $03A52E) on the swaying body $037DD6 (oscillator X $40/$55/$55/$78, as ToyChain_MakeBox10A's)
; In:  a6 = the holder (field $2A or $2C of the Op2_SpawnChild that names it); the inline arguments after the jsr
; Out: d0 = 0 and the chain object, or nonzero (pool full)
; Evidence: Op2_SpawnChild $2A dc.l at $03A292 (script $03A270); its only namer is the script of the raw stub $03A264 (lea script / jmp Obj_SpawnEnemy), which no ROM long, table or op operand names: no reachable caller is known (the long at $084DE6 is art); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
ToyChain_MakeBox10D:
	jsr	ToyChain_SpawnInline(pc)
