; ---------------------------------------------------------------------------
; Scene_RunList - calls each scene routine of a zero-terminated long list in turn while g_SceneResult stays 0
; In:  a0 = list of routine longs, 0 ends
; Out: returns at the end of the list or after the first scene that leaves a nonzero result
; Evidence: pea (a0) / jsr (a0) / cmpi.w #0,$F608; lists Tbl_00A136, Tbl_00A152 (Main_*), Tbl_00A166-Tbl_00A18A (Stage_Intro*); 7 TAS frames
; ---------------------------------------------------------------------------
Scene_RunList:
	move.l	(a0)+,d0
	beq.s	loc_009FFA
	pea.l	(a0)
	movea.l	d0,a0
	jsr	(a0)
	movea.l	(a7)+,a0
	cmpi.w	#$0,(g_SceneResult).w
	beq.s	Scene_RunList
loc_009FFA:
	rts
