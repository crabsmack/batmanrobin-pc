; ---------------------------------------------------------------------------
; Op6_ClearPlane - a6 op: fills a whole VDP plane with the blank tile $5FC through a V-int job (a few rows per V-int), parking the script until it is done
; In:  a6 = script: word address of the plane VRAM variable (g_PlaneAVRAM / g_PlaneBVRAM)
; Out: parks (VM6_StartClearJob)
; Evidence: d0-d3 = 0 (VM6_SetupClearRect: whole plane) then bra VM6_StartClearJob; TAS f77148 only (1 run); the raw $721E/$722E twins end in the unrun direct writer $7296
; ---------------------------------------------------------------------------
Op6_ClearPlane:
	moveq	#$0,d0
	moveq	#$0,d1
	moveq	#$0,d2
	moveq	#$0,d3
	bsr.w	VM6_SetupClearRect
	bra.w	VM6_StartClearJob
; ---------------------------------------------------------------------------
; Op6_ClearPlaneRect - a6 op: fills a rectangle of a VDP plane with the blank tile $5FC through a V-int job, parking the script until it is done
; In:  a6 = script: words x (bytes), y (cell row), width, height (cells; 0 = plane size), word address of the plane VRAM variable
; Out: parks (VM6_StartClearJob)
; Evidence: movem.w (a6)+,d0-d3 / bsr VM6_SetupClearRect / bra VM6_StartClearJob; TAS f32080 and fuzz-30600-s2
; ---------------------------------------------------------------------------
Op6_ClearPlaneRect:
	movem.w	(a6)+,d0-d3
	bsr.w	VM6_SetupClearRect
	bra.w	VM6_StartClearJob
