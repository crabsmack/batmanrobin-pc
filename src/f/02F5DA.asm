; ---------------------------------------------------------------------------
; Flight_AirshipMapToWorld - places a map-anchored object: world x/y ($3E/$42) = its map position ($4E/$50) - g_SceneVars+$94/$96, so it stays fixed on the scrolling airship map
; In:  a6 = object with its map position in $4E/$50
; Out: $3E/$42 (whole pixels) set; d4/d5 = the world position
; Evidence: op $2CB2 in every body of the section (lights $2F74A, hull turret $2F9B0, domes $2FC86, ball turret $2FE10, $30114, $30562-$305D2) and from Flight_AirshipRunTriggers / Flight_AirshipStoreMapPos; TAS 67264-76867 (9566 frames)
; ---------------------------------------------------------------------------
Flight_AirshipMapToWorld:
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	sub.w	(g_SceneVars+$94).w,d4
	sub.w	(g_SceneVars+$96).w,d5
	move.w	d4,$3E(a6)
	move.w	d5,$42(a6)
	rts
