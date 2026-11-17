; ---------------------------------------------------------------------------
; Boss_Hatter_LittleHatDropLife - a2 native fragment (op $07FC at $03D9AC) when the little hat is shot down: runs spawn action $1C (Item_SpawnExtraLifeFromHit) at its position; the script then kills it (Op2_Kill at $03D9BC)
; In:  a6 = the little hat
; Out: as SpawnAction_Run
; Evidence: after Op2_TakeHit and Op2_FxSpawnOffAbs in Scr2_03D940; Tbl_012EF0 entry 28 = $0135A2; TAS f115744 (one frame); png f115800: 1UP beside Batman
; ---------------------------------------------------------------------------
Boss_Hatter_LittleHatDropLife:
	move.w	#$1C,d0
	jsr	(SpawnAction_Run).l
	jsr	($7FE).w
