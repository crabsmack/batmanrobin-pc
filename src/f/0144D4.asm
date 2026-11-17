; ---------------------------------------------------------------------------
; TextMap_Clear - fills the 40x28 plane image at $FF9000 with the blank tile $05FC
; In:  none
; Out: d0/d7/a0 trashed
; Evidence: 56 x 10 longs of $05FC05FC = 1120 words = 40x28; called by the menu scenes before their first TextMap_Run ($499B6, $499E8); scenario-ending
; ---------------------------------------------------------------------------
TextMap_Clear:
	lea.l	(g_SceneWorkBuf).l,a0
	move.l	#$5FC05FC,d0
	moveq	#$37,d7
loc_0144E2:
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	dbra	d7,loc_0144E2
	rts
; ---------------------------------------------------------------------------
; TextMap_Upload - queues DMAs of the 28 rows of the plane image to plane A (g_PlaneAVRAM), 40 words per row
; In:  none
; Out: d0-d7/a0 trashed; stops early when the DMA queue is full
; Evidence: moveq #$1B,d7 rows; DMA_Reserve per row; length $28 words from $FF9000 ($95/$96/$97 = $00/$C8/$7F), row stride g_PlaneRowBytes; every frame after TextMap_Run in the menu scenes
; ---------------------------------------------------------------------------
TextMap_Upload:
	moveq	#$1B,d7
	move.w	(g_PlaneAVRAM).w,d2
; ---------------------------------------------------------------------------
; TextMap_UploadRows - queues DMAs of d7+1 rows of the plane image to a given VRAM address (40 words per row, the plane's row stride apart)
; In:  d2 = VRAM address, d7 = rows - 1
; Out: d0-d7/a0 trashed
; Evidence: the loop of TextMap_Upload; $49212 sends 3 rows to g_PlaneBVRAM + $C00
; ---------------------------------------------------------------------------
TextMap_UploadRows:
	andi.l	#$FFFF,d2
	lsl.l	#$2,d2
	lsr.w	#$2,d2
	swap	d2
	ori.l	#$40000080,d2
	move.w	#$9500,d3
	move.w	#$96C8,d4
	move.w	#$977F,d5
	move.w	(g_PlaneRowBytes).w,d6
	add.w	d6,d6
	add.w	d6,d6
loc_014528:
	moveq	#$1,d0
	moveq	#$3,d1
	jsr	(DMA_Reserve).l
	bmi.s	loc_014560
	move.l	#$93289400,(a0)+
	move.w	d3,(a0)+
	move.w	d4,(a0)+
	move.w	d5,(a0)+
	move.l	d2,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
	addi.b	#$28,d3
	bcc.s	loc_014552
	addq.b	#$1,d4
	bcc.s	loc_014552
	addq.b	#$1,d5
loc_014552:
	swap	d2
	rol.w	#$2,d2
	add.w	d6,d2
	ror.w	#$2,d2
	swap	d2
	dbra	d7,loc_014528
loc_014560:
	rts
; ---------------------------------------------------------------------------
; Menu_MoveCursor - moves the menu cursor g_MenuCursor up/down with the D-pad presses, clamped to [0, d1]
; In:  d1 = last item
; Out: d0 = cursor, d2 = pad 1 newly pressed byte
; Evidence: g_Pad1+2 bit 0 (up) subq, bit 1 (down) addq, clamp; callers $499C2 (d1 = 1), Menu_Update
; ---------------------------------------------------------------------------
Menu_MoveCursor:
	move.w	(g_MenuCursor).w,d0
	move.b	(g_Pad1+2).w,d2
	btst	#$0,d2
	beq.s	loc_014578
	subq.w	#$1,d0
	bpl.s	loc_014586
	moveq	#$0,d0
	bra.s	loc_014586
loc_014578:
	btst	#$1,d2
	beq.s	loc_014586
	addq.w	#$1,d0
	cmp.w	d1,d0
	ble.s	loc_014586
	move.w	d1,d0
loc_014586:
	move.w	d0,(g_MenuCursor).w
	rts
