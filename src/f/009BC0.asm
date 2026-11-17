; ---------------------------------------------------------------------------
; Frame_RunWorld - runs one gameplay step: the stage trigger table (Trigger_RunX), every object (Obj_RunList) and camera tracking (Camera_TrackPlayersX); with the debug free camera on it moves the camera instead
; In:  none
; Out: objects stepped; g_CameraX advanced
; Evidence: tst g_FreeCam ($F4F1) -> Frame_RunFreeCam; callers $1FB3A, $3FFBE, $475C2, $47772, $479EE, $47BD0 (section loops); 21173 TAS frames
; ---------------------------------------------------------------------------
Frame_RunWorld:
	tst.b	(g_FreeCam).w
	bne.s	Frame_RunFreeCam
	jsr	(Trigger_RunX).l
	jsr	Obj_RunList(pc)
	jmp	Camera_TrackPlayersX(pc)
; ---------------------------------------------------------------------------
; Frame_RunWorldLayerA - like Frame_RunWorld but ends with Camera_TrackPlayersXLayerA ($12790), which tracks the players and also stores plane A's offset from the view (g_LayerA+4/+6 = view - layer position)
; In:  none
; Out: objects stepped, camera and g_LayerA+4/+6 updated
; Evidence: $12790 starts with jsr Camera_TrackPlayersX; g_LayerA+4 = g_ViewX - g_LayerA+0, +6 = g_ViewY - +2; callers $1FC50, $1FD76, $1FECC, $3FE94, $400E4, $403F6 (sections with a scrolling plane A layer)
; ---------------------------------------------------------------------------
Frame_RunWorldLayerA:
	tst.b	(g_FreeCam).w
	bne.s	Frame_RunFreeCam
	jsr	(Trigger_RunX).l
	jsr	Obj_RunList(pc)
	jmp	(Camera_TrackPlayersXLayerA).l
; ---------------------------------------------------------------------------
; Frame_RunFreeCam - debug free-camera step taken instead of the objects while g_FreeCam is set: the pad moves the camera ($15730)
; In:  none
; Out: see $15730 (pad direction accelerates the camera; C toggles g_CameraLockX)
; Evidence: branched to by Frame_RunWorld and Frame_RunWorldLayerA when $F4F1 != 0; $F4F1 is toggled by B in the debug pause menu $1569C
; ---------------------------------------------------------------------------
Frame_RunFreeCam:
	jmp	(Debug_FreeCamStep).l
