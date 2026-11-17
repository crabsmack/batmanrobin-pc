; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdTbl_Goto - entry 2 of Boss_Hatter_RoadCmdTbl (goto): bra.s to Boss_Hatter_RoadCmdGoto
; In:  as Boss_Hatter_RoadCmdTbl
; Out: -
; Evidence: $03DD32 + 2 x 2; ctarget fuzz-113400-s2 (the schedule's F2E8 loop)
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdTbl_Goto:
	bra.s	Boss_Hatter_RoadCmdGoto
