; ---------------------------------------------------------------------------
; Stage3_SpottedRoomScroll - per-frame scroll of the spotted room: swings the wobble spring (g_Stage3RoomSway, speed +-4 toward 0), slides the pattern (g_Stage3RoomSlide, speed down to -4.0, wrapped to +-40 px), then writes 224 plane-A line H-scrolls (88 ceiling lines spreading by slide/$92 per line, 48 back-wall lines, 88 floor lines; all sheared by the wobble) and 20 column V-scrolls (sheared by the wobble x 16 per column) into g_SceneWorkBuf
; In:  -
; Out: g_HScrollLinesA (224 words), g_SceneWorkBuf (20 words: column V-scroll of plane A for mode $2C), the four g_Stage3Room* globals
; Evidence: jsr from Stage3_RunSpottedRoom's frame loop; loop counts $57/$2F/$57 and $13; TAS f101686-f108293 (6476 frames); png f102000 vs f105000: the room's lines and columns lean differently
; ---------------------------------------------------------------------------
Stage3_SpottedRoomScroll:
	move.w	#$4,d0
	move.w	(g_Stage3RoomSwayVel).l,d1
	move.w	(g_Stage3RoomSway).l,d2
	bmi.s	loc_040286
	neg.w	d0
loc_040286:
	add.w	d0,d1
	move.w	d1,(g_Stage3RoomSwayVel).l
	add.w	d1,d2
	move.w	d2,(g_Stage3RoomSway).l
	move.l	(g_Stage3RoomSlideVel).l,d0
	move.l	#$1000,d1
	move.l	#$40000,d3
	neg.l	d3
	cmp.l	d3,d0
	ble.s	loc_0402B0
	sub.l	d1,d0
loc_0402B0:
	move.l	d0,(g_Stage3RoomSlideVel).l
	add.l	(g_Stage3RoomSlide).l,d0
	cmpi.l	#$280000,d0
	blt.s	loc_0402CA
	subi.l	#$500000,d0
loc_0402CA:
	cmpi.l	#$FFD80000,d0
	bgt.s	loc_0402D8
	addi.l	#$500000,d0
loc_0402D8:
	move.l	d0,(g_Stage3RoomSlide).l
	bra.w	loc_0402E2
loc_0402E2:
	lea.l	(g_HScrollLinesA).l,a2
	lea.l	(g_SceneWorkBuf).l,a3
	move.w	#$70,d2
	move.w	#$A8,d3
	move.w	(g_Stage3RoomSway).l,d1
	ext.l	d1
	muls.w	d1,d3
	neg.l	d3
	muls.w	d1,d2
	neg.l	d2
	move.l	(g_Stage3RoomSlide).l,d4
	move.l	d4,d6
	divs.w	#$92,d6
	ext.l	d6
	addi.l	#$FFE00000,d4
	move.w	#$57,d7
loc_04031E:
	move.l	d4,d5
	add.l	d2,d5
	swap	d5
	move.w	d5,(a2)+
	add.l	d1,d2
	sub.l	d6,d4
	dbra	d7,loc_04031E
	move.w	#$2F,d7
loc_040332:
	move.l	d4,d5
	add.l	d2,d5
	swap	d5
	move.w	d5,(a2)+
	add.l	d1,d2
	dbra	d7,loc_040332
	move.w	#$57,d7
loc_040344:
	move.l	d4,d5
	add.l	d2,d5
	swap	d5
	move.w	d5,(a2)+
	add.l	d1,d2
	add.l	d6,d4
	dbra	d7,loc_040344
	asl.l	#$4,d1
	addi.l	#$100000,d3
	move.w	#$13,d7
loc_040360:
	swap	d3
	move.w	d3,(a3)+
	swap	d3
	add.l	d1,d3
	dbra	d7,loc_040360
	rts
