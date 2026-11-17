; ---------------------------------------------------------------------------
; Stage3_SpottedRoomInitScroll - starts the spotted room's scroll: scroll mode $2C (line H-scroll + 2-cell column V-scroll), wobble g_Stage3RoomSway = $B00 with speed $20
; In:  -
; Out: g_ScrollMode, g_Stage3RoomSway, g_Stage3RoomSwayVel
; Evidence: jsr from Stage3_RunSpottedRoom only; TAS f101686 (one frame)
; ---------------------------------------------------------------------------
Stage3_SpottedRoomInitScroll:
	move.w	#$2C,(g_ScrollMode).l
	move.w	#$B00,(g_Stage3RoomSway).l
	move.w	#$20,(g_Stage3RoomSwayVel).l
	rts
