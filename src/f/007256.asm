; ---------------------------------------------------------------------------
; VM6_SetupClearRect - a6 helper: sets up a plane fill job: fill tile $5FC, the VDP write command of the rectangle's first row and the column/row counts (0 = the whole plane width/height)
; In:  d0 = x (bytes), d1 = row, d2 = width, d3 = height (cells, 0 = g_PlaneCellsX/Y), a6 = script: word address of the plane VRAM variable
; Out: g_A6TileWord = $5FC, g_A6VdpCmd = VRAM write command for base + x + row * g_PlaneRowBytes, g_A6RectCols = width-1, g_A6RectRows = height-1; g_A6ScriptPc = a6 past the word
; Evidence: callers Op6_ClearPlane / Op6_ClearPlaneRect (and the raw $721E/$722E); lsl.l #2 / lsr.w #2 / ori #$4000 / swap = VDP VRAM-write command; 2 corpus runs
; ---------------------------------------------------------------------------
VM6_SetupClearRect:
	move.w	#$5FC,(g_A6TileWord).w
	movea.w	(a6)+,a0
	add.w	(a0),d0
	mulu.w	(g_PlaneRowBytes).w,d1
	add.w	d0,d1
	lsl.l	#$2,d1
	lsr.w	#$2,d1
	ori.w	#$4000,d1
	swap	d1
	move.l	d1,(g_A6VdpCmd).w
	tst.w	d2
	bne.s	loc_00727C
	move.w	(g_PlaneCellsX).w,d2
loc_00727C:
	tst.w	d3
	bne.s	loc_007284
	move.w	(g_PlaneCellsY).w,d3
loc_007284:
	subq.w	#$1,d2
	move.w	d2,(g_A6RectCols).w
	subq.w	#$1,d3
	move.w	d3,(g_A6RectRows).w
	move.l	a6,(g_A6ScriptPc).w
	rts
