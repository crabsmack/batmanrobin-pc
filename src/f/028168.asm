; ---------------------------------------------------------------------------
; Enemy_DomeCraft_TakeFireRequest - native switch in the dome craft's animation (body $0280EC: contact attack box, fires by facing): returns bit 0 of script var 0 (1 = a shot was requested) and clears var 0
; In:  a4 +$22 var 0
; Out: d0 = 0 / 1 for Op2_CallNativeRelSwitch
; Evidence: op $2D30 at $0281FA and $02823A (case 1: muzzle frame + Op2_FxSpawnRegs of the a4 effect at $028176 by facing); var 0 bumped by Op2_IncVar0 at $028164; body $0280EC is entered by Enemy_DomeCraft_SpawnFromLeft/SpawnA/SpawnB (green-domed craft, f44400); RAM f41700: such an object (loop $028134); TAS f41668-f65089 (209)
; ---------------------------------------------------------------------------
Enemy_DomeCraft_TakeFireRequest:
	move.w	$22(a4),d0
	clr.w	$22(a4)
	andi.w	#$1,d0
	rts
