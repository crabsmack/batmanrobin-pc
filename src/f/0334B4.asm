; ---------------------------------------------------------------------------
; Flight_BuildWobbleTable - builds the cloud bank's wobble table at g_SceneWorkBuf+$400: 256 words = one sine period (every second word of Tbl_059B22, high byte / 16: +-8 px), then 224 words repeating its start so any 224-line window can be read without wrapping
; In:  -
; Out: g_SceneWorkBuf+$400..+$7BF filled; a0/a1/d0/d7 used
; Evidence: called once by Flight_StartClouds (TAS f46290); read by Flight_ScrollCloudBank's line loop
; ---------------------------------------------------------------------------
Flight_BuildWobbleTable:
	lea.l	(Tbl_059B22).l,a0
	lea.l	(g_SceneWorkBuf+$400).l,a1
	move.w	#$FF,d7
	bsr.w	Flight_CopyWobbleWords
	lea.l	(Tbl_059B22).l,a0
	move.w	#$DF,d7
