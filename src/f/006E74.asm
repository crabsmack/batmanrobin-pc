; ---------------------------------------------------------------------------
; Op5_LockCameraHere - a5 op: locks the camera where it is: g_CameraLockX = g_CameraX + $A0 (16.16), so Camera_TrackPlayersX holds the view for an arena fight
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: (g_CameraX + $A0) << 16 -> g_CameraLockX (as Camera_LockHere); 35 script uses, e.g. $16B40 (`6E74 6ECE`), $16BE0; 9 corpus runs
; ---------------------------------------------------------------------------
Op5_LockCameraHere:
	moveq	#$0,d0
	move.w	(g_CameraX).w,d0
	addi.w	#$A0,d0
	swap	d0
	move.l	d0,(g_CameraLockX).w
	movea.w	(a5)+,a0
	jmp	(a0)
