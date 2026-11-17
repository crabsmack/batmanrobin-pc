; ---------------------------------------------------------------------------
; Fx_RunScript - runs a4 spawn script a0 on g_SpawnCtx with the parent's velocity and flip, so the objects it spawns start from d2-d5
; In:  a0 = a4 script, d2/d3/d4/d5 = radius, heading, x, y (16.16) for the new objects, a3 = parent motion block, a5 = parent sprite block
; Out: a1-a6 preserved
; Evidence: ctx +$36/$3A cleared, +$3E/$42 = a3+$18/$1C (velocity), +$22 = a5+$38 (attr); VM4_Next enters the a4 VM; spawn op $613A copies d2-d5 to the new object's $26/$2A/$2E/$32; callers $1088A, $10D12, $1A34E, $3CCD4...; 2456 TAS frames
; ---------------------------------------------------------------------------
Fx_RunScript:
	movem.l	a1-a6,-(a7)
	movea.l	a0,a4
	lea.l	(g_SpawnCtx).w,a6
	clr.l	$36(a6)
	clr.l	$3A(a6)
	move.l	$18(a3),$3E(a6)
	move.l	$1C(a3),$42(a6)
	move.w	$38(a5),$22(a6)
	bsr.w	VM4_Next
	movem.l	(a7)+,a1-a6
	rts
; ---------------------------------------------------------------------------
; Fx_SpawnAtScreenCentre - runs the a4 spawn script at a4 with the new objects at the screen centre (camera + $A0, $70)
; In:  a4 = a4 script
; Out: via Fx_RunScriptNoParent
; Evidence: d2 = d3 = 0, d4/d5 = g_CameraX/Y + $A00000/$700000; caller $20D76
; ---------------------------------------------------------------------------
Fx_SpawnAtScreenCentre:
	moveq	#$0,d2
	moveq	#$0,d3
	move.l	(g_CameraX).l,d4
	move.l	(g_CameraY).l,d5
	addi.l	#$A00000,d4
	addi.l	#$700000,d5
