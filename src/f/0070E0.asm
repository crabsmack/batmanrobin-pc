; ---------------------------------------------------------------------------
; Op6_CallNative - a6 op: calls a native routine (dc.l) with a6 at its inline arguments and continues after it
; In:  a6 = script: long routine, then the routine's own inline words
; Out: continues
; Evidence: 6 script uses: `70E0 0000 8580 000D` ($1FB30, ArtCache_AddBlocksInline $D), `70E0 0000 772E 2300` ($33050, Section_SetIdInline); TAS f4539-77148
; ---------------------------------------------------------------------------
Op6_CallNative:
	movea.l	(a6)+,a0
	jsr	(a0)
	bra.s	VM6_Next
; ---------------------------------------------------------------------------
; Op6_InitBankFrames - a6 op: builds the RAM sprite-frame entries for a sprite bank already loaded into a scene buffer (Spr_InitBankFrames with handle base $8002)
; In:  a6 = script: word g_BufPtrs offset (the bank), word frame-table offset d0 (from $A764)
; Out: continues
; Evidence: 12 script uses, e.g. $1FB26 `70E6 0008 FC40` after `7120 0008 0006 E042` decompressed the bank into buffer 8; d1 = g_A6BankHandleBase ($8002 every time); ran on 16 TAS frames (bank loads)
; ---------------------------------------------------------------------------
Op6_InitBankFrames:
	move.w	#$8002,(g_A6BankHandleBase).w
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	move.w	(a6)+,d0
	move.w	(g_A6BankHandleBase).w,d1
	jsr	(Spr_InitBankFrames).l
	move.w	d1,(g_A6BankHandleBase).w
	bra.s	VM6_Next
; ---------------------------------------------------------------------------
; Op6_Decompress - a6 op: decompresses an LZ block into a scene buffer synchronously, then continues
; In:  a6 = script: word g_BufPtrs offset (destination), long LZ block
; Out: continues (a6 reloaded); g_LzOutEnd = end of the output
; Evidence: Lz_Decompress with a1 = buffer, a0 = block; 61 script uses, e.g. Tbl_01FAB2 `7108 0000 0005 E63C`, $1FC06 `7108 0000 0006 2180`; 24 corpus runs
; ---------------------------------------------------------------------------
Op6_Decompress:
	lea.l	(g_BufPtrs).l,a1
	adda.w	(a6)+,a1
	movea.l	(a1),a1
	movea.l	(a6)+,a0
	move.l	a6,(g_A6ScriptPc).w
	jsr	(Lz_Decompress).l
	bra.s	VM6_Reload
; ---------------------------------------------------------------------------
; Op6_DecompressBg - a6 op: decompresses an LZ block into a scene buffer as the background task (BgTask_Start) and parks until it finishes
; In:  a6 = script: word g_BufPtrs offset (destination), long LZ block
; Out: parks (g_A6ResumeHook = VM6_WaitBgDecompress); the decompressor runs while the main loop waits for the V-int
; Evidence: 14 script uses, e.g. $1FB1E `7120 0008 0006 E042` (stage 1 sprite bank, then Op6_InitBankFrames), $1FD62 `7120 0004 0007 3FBA`; jmp BgTask_Start; 10 corpus runs
; ---------------------------------------------------------------------------
Op6_DecompressBg:
	lea.l	(g_BufPtrs).l,a1
	adda.w	(a6)+,a1
	movea.l	(a1),a1
	movea.l	(a6)+,a0
	move.l	a6,(g_A6ScriptPc).w
	move.l	#$713E,(g_A6ResumeHook).w
	jmp	(BgTask_Start).l
; ---------------------------------------------------------------------------
; VM6_WaitBgDecompress - a6 resume hook of Op6_DecompressBg: returns while g_BgTaskActive is set, then resumes the script
; In:  -
; Out: rts (still waiting) or VM6_Resume
; Evidence: hook value $713E set by Op6_DecompressBg; tst g_BgTaskActive (cleared by the decompressor when done); 10 corpus runs
; ---------------------------------------------------------------------------
VM6_WaitBgDecompress:
	tst.b	(g_BgTaskActive).w
	beq.w	VM6_Resume
	rts
; ---------------------------------------------------------------------------
; Op6_StoreLzEnd - a6 op: stores the end of the last decompression (g_LzOutEnd) into a g_BufPtrs slot, so the next block can be unpacked right after it
; In:  a6 = script: word g_BufPtrs offset
; Out: continues
; Evidence: 6 script uses, e.g. $1FB2C `7148 000C` after a decompression into slot 8, $1FD70 `7148 0008`; 12 corpus runs
; ---------------------------------------------------------------------------
Op6_StoreLzEnd:
	lea.l	(g_BufPtrs).l,a0
	adda.w	(a6)+,a0
	move.l	(g_LzOutEnd).w,(a0)
	bra.w	VM6_Next
; ---------------------------------------------------------------------------
; Op6_SetBufPtrToEntry - a6 op: points one g_BufPtrs slot at an entry inside another slot's buffer (the buffer starts with self-relative long offsets to its entries)
; In:  a6 = script: word source slot offset, word entry offset in that buffer, word destination slot offset
; Out: continues; g_BufPtrs[dest] = entry + (long at entry)
; Evidence: 11 script uses, e.g. $1FC3E `7158 0000 0008 0004` and $1FC46 `7158 0000 0004 0008`; adda.l (a0),a0; 15 corpus runs
; ---------------------------------------------------------------------------
Op6_SetBufPtrToEntry:
	lea.l	(g_BufPtrs).l,a0
	movea.l	a0,a1
	adda.w	(a6)+,a0
	movea.l	(a0),a0
	adda.w	(a6)+,a0
	adda.l	(a0),a0
	adda.w	(a6)+,a1
	move.l	a0,(a1)
	bra.w	VM6_Next
; ---------------------------------------------------------------------------
; Op6_LoadPalettesYield - a6 op: loads palettes from a buffer entry into the lines picked by a mask (VM6_LoadPalettes), then yields the script until the next frame
; In:  a6 = script: word g_BufPtrs offset, word entry offset, byte count-1, byte line mask (2 bits per palette from bit 7)
; Out: parks (g_A6ResumeHook = VM6_Resume)
; Evidence: bsr VM6_LoadPalettes then g_A6ScriptPc = a6 / hook $70A8 / rts; no use in the decoded scripts, ran in 6 corpus runs
; ---------------------------------------------------------------------------
Op6_LoadPalettesYield:
	bsr.w	VM6_LoadPalettes
	move.l	a6,(g_A6ScriptPc).w
	move.l	#$70A8,(g_A6ResumeHook).w
	rts
; ---------------------------------------------------------------------------
; Op6_LoadPalettes - a6 op: loads palettes from a buffer entry into the lines picked by a mask (VM6_LoadPalettes) and continues
; In:  a6 = script: word g_BufPtrs offset, word entry offset, byte count-1, byte line mask (2 bits per palette from bit 7: 0-3 = line)
; Out: continues (pea VM6_Next, then the helper's rts)
; Evidence: 32 script uses, e.g. $1FC36 `7182 0000 0004 0040` (one palette to line 1); 24 corpus runs
; ---------------------------------------------------------------------------
Op6_LoadPalettes:
	pea.l	VM6_Next(pc)
