; ---------------------------------------------------------------------------
; Item_CollectExtraLife - touch native of the extra-life pickup: adds one life to the collecting player
; In:  g_FoundBox = the toucher's box
; Out: player record +6 lives + 1
; Evidence: op $2CB2 at $135EA after sound $42, followed by op $518C (an effect at the pickup); +6 = lives (NOTES Player records); 4 TAS frames (31540-115798)
; ---------------------------------------------------------------------------
Item_CollectExtraLife:
	bsr.w	Item_FoundBoxPlayer
	addq.w	#$1,$6(a0)
	rts
