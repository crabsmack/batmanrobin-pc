; ---------------------------------------------------------------------------
; Stage_Intro1 - section $0001: stage 1's opening, stage music 8 then the scene list Tbl_00A166 ($4A180 shared stage opening, $4A216 stage 1)
; In:  none
; Out: returns when the list ends or a scene sets g_SceneResult
; Evidence: Tbl_00A0AC maps section $0001 to $9F74; music 8 is also what section $1100 ($1FB3A) plays; TAS f477
; ---------------------------------------------------------------------------
Stage_Intro1:
	move.w	#$8,d0
	bsr.w	Sound_PlayMusic
	lea.l	(Tbl_00A166).l,a0
	bra.w	Scene_RunList
; ---------------------------------------------------------------------------
; Stage_Intro2 - section $0002: stage 2's opening, music $C and Tbl_00A172 ($4A180, $4A2DC)
; In:  none
; Out: as Stage_Intro1
; Evidence: Tbl_00A0AC maps $0002 to $9F86; it follows section $1400 in Tbl_009FFC
; ---------------------------------------------------------------------------
Stage_Intro2:
	move.w	#$C,d0
	bsr.w	Sound_PlayMusic
	lea.l	(Tbl_00A172).l,a0
	bra.w	Scene_RunList
; ---------------------------------------------------------------------------
; Stage_Intro3 - section $0003: stage 3's opening, music $E and Tbl_00A17E ($4A180, $4A3A4)
; In:  none
; Out: as Stage_Intro1
; Evidence: Tbl_00A0AC maps $0003 to $9F98; it follows section $2401 in Tbl_009FFC
; ---------------------------------------------------------------------------
Stage_Intro3:
	move.w	#$E,d0
	bsr.w	Sound_PlayMusic
	lea.l	(Tbl_00A17E).l,a0
	bra.w	Scene_RunList
; ---------------------------------------------------------------------------
; Stage_Intro4 - section $0004: stage 4's opening, music $B and Tbl_00A18A ($4A180, $4A464)
; In:  none
; Out: as Stage_Intro1
; Evidence: Tbl_00A0AC maps $0004 to $9FAA; it follows section $3401 in Tbl_009FFC
; ---------------------------------------------------------------------------
Stage_Intro4:
	move.w	#$B,d0
	bsr.w	Sound_PlayMusic
	lea.l	(Tbl_00A18A).l,a0
	bra.w	Scene_RunList
