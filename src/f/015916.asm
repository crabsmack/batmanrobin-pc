; ---------------------------------------------------------------------------
; Debug_FreeCamScroll - tail of Debug_FreeCamStep: scrolls the camera right by up to 4 px toward the cursor (or the lock x), resyncs layer A and updates the view
; In:  d0 = pixels the cursor is right of the screen centre (> 0)
; Out: g_CameraX advanced (cleared if negative), g_LayerA +4 = g_CameraX - layer x, Camera_UpdateView
; Evidence: cmpi.w #4 clamp, add.w d0,g_CameraX, jmp Camera_UpdateView; static only
; ---------------------------------------------------------------------------
Debug_FreeCamScroll:
	cmpi.w	#$4,d0
	blt.s	loc_015920
	move.w	#$4,d0
loc_015920:
	add.w	d0,(g_CameraX).w
	bpl.s	Debug_FreeCamSyncLayer
	clr.l	(g_CameraX).w
; ---------------------------------------------------------------------------
; Debug_FreeCamSyncLayer - tail of Debug_FreeCamScroll and the free camera's no-scroll exit: plane A's pending x = camera x - its x, then Camera_UpdateView
; In:  g_CameraX, g_LayerA
; Out: a6 = g_LayerA, its +4 = g_CameraX - its x (word); then as Camera_UpdateView
; Evidence: lea g_LayerA,a6 / move.w g_CameraX,d0 / sub.w (a6),d0 / move.w d0,4(a6) / jmp Camera_UpdateView; bra.s from Debug_FreeCamStep (cursor left of centre) and the fall from Debug_FreeCamScroll; static only
; ---------------------------------------------------------------------------
Debug_FreeCamSyncLayer:
	lea.l	(g_LayerA).l,a6
	move.w	(g_CameraX).w,d0
	sub.w	(a6),d0
	move.w	d0,$4(a6)
	jmp	(Camera_UpdateView).l
