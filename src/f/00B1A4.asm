; ---------------------------------------------------------------------------
; HInt_InstallVScrollB - installs the per-line V-scroll raster for plane B: the H-int handler will write one word per line to VSRAM 2
; In:  none
; Out: g_HIntVSRAMCmd = $40020010; then as HInt_Install
; Evidence: VSRAM write command for address 2 (plane B's V-scroll); callers $33274, $33322 (stage 2 section code) and $3BB88 (with modes 4/$3C); 4 corpus runs, TAS frames 35432-89521
; ---------------------------------------------------------------------------
HInt_InstallVScrollB:
	move.l	#$40020010,(g_HIntVSRAMCmd).w
	bra.s	HInt_Install
; ---------------------------------------------------------------------------
; HInt_InstallVScrollA - installs the per-line V-scroll raster for plane A (VSRAM 0)
; In:  none
; Out: g_HIntVSRAMCmd = $40000010; then as HInt_Install
; Evidence: callers $334D2 and $471C0 (mode $38, span effects), $405F2 (mode $28), $3BB88
; ---------------------------------------------------------------------------
HInt_InstallVScrollA:
	move.l	#$40000010,(g_HIntVSRAMCmd).w
; ---------------------------------------------------------------------------
; HInt_Install - copies the H-int handler from Tbl_HIntHandlerSrc to RAM g_HIntCode and sets the two line tables: g_HIntTblBack = $9040, g_HIntTblFront = $9240
; In:  g_HIntVSRAMCmd set by the caller
; Out: handler at $FFE560 (38 words; only $26 bytes are the handler), d7 = -1
; Evidence: `lea Tbl_HIntHandlerSrc,a0; lea $FFE560,a1; moveq #$25,d7` word copy; the IRQ4 vector points to $FFE560; tail of HInt_InstallVScrollA/B
; ---------------------------------------------------------------------------
HInt_Install:
	lea.l	(Tbl_HIntHandlerSrc).l,a0
	lea.l	(g_HIntCode).l,a1
	moveq	#$25,d7
loc_00B1C4:
	move.w	(a0)+,(a1)+
	dbra	d7,loc_00B1C4
	move.w	#$9040,(g_HIntTblBack).w
	move.w	#$9240,(g_HIntTblFront).w
	rts
; ---------------------------------------------------------------------------
; HInt_Rearm - re-arms the H-int raster on the table already shown: points the handler at g_HIntTblFront again, then HInt_Arm
; In:  a0 = VDP control port
; Out: as HInt_Arm
; Evidence: `move.w $E552,$E562`; from the busy-path table (VInt_BusyScrollTbl34, VInt_BusyRearmRaster): a frame that wasn't rebuilt replays the previous lines
; ---------------------------------------------------------------------------
HInt_Rearm:
	move.w	(g_HIntTblFront).w,(g_HIntOperand).w
	bra.s	HInt_Arm
; ---------------------------------------------------------------------------
; HInt_SwapRearm - swaps the double-buffered line tables (the one the game just filled becomes g_HIntTblFront) and arms the H-int on it
; In:  a0 = VDP control port
; Out: g_HIntTblFront/g_HIntTblBack exchanged, the handler's operand = new front; then HInt_Arm
; Evidence: exchanges $E552/$E554 and stores the new $E552 to $E562; the span renderers ($F96A, $FE0E) and $3BB88 fill the table `movea.w $E554,a1`; branch target of the raster modes $28/$34/$38/$3C; 35 corpus runs
; ---------------------------------------------------------------------------
HInt_SwapRearm:
	move.w	(g_HIntTblBack).w,d0
	move.w	(g_HIntTblFront).w,d1
	move.w	d0,(g_HIntTblFront).w
	move.w	d1,(g_HIntTblBack).w
	move.w	d0,(g_HIntOperand).w
; ---------------------------------------------------------------------------
; HInt_Arm - enables the per-line H-int: restores the handler's continue branch, sets H-int on every line, auto-increment 0 and the display on, and points the VDP at the handler's VSRAM target
; In:  a0 = VDP control port; g_HIntOperand = table address
; Out: VDP reg 0 = $14, reg 10 = 0, reg 15 = 0, reg 1 = $64, VDP address = g_HIntVSRAMCmd
; Evidence: `move.b #2,$E56D` (bcs displacement back to the high-byte step), `move.l #$80148A00`, `move.l #$8F008164`, `move.l $E556,(a0)`; with increment 0 every handler write lands on the same VSRAM word, giving one V-scroll per line; 35 corpus runs
; ---------------------------------------------------------------------------
HInt_Arm:
	move.b	#$2,(g_HIntBcsDisp).w
	move.l	#$80148A00,(a0)
	move.l	#$8F008164,(a0)
	move.l	(g_HIntVSRAMCmd).w,(a0)
	rts
