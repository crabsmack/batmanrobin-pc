; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn1A - spawns a stage 4 hall hover pod on course 1 ($44836: two random screen-area waypoints, x cruise $2AA then $22A/$200) with attack A ($44D68: three rounds of six aimed shots $A0 ticks apart)
; In:  d4/d5 = x/y (wave entry: screen x $A0, y $70)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_0444FC: speed $100, drop init $43A04, score $1F4, energy $1C0 (1P and 2P), x accel/cruise $2AA, y $155, coroutine 0 $44836, coroutine 1 $44D68, Op2_JitterPos $12C/$28 and op $1640 -$96 (starts above the screen), body $42152 (oscillator hover, heading-switched frames $48F8-$4970); wave entries in Tbl_041138 (+$6E, +$A2, +$166, +$1DC: hall waves started from Tbl_040F4A); RAM f117300: the pod (slots $4483E/$44D78 = course 1, attack A) at (151, 38) = the purple striped sphere with a thruster jet in shot w29_s117300; TAS 116492-117218
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn1A:
	lea.l	(Scr2_0444FC).l,a2
	jmp	(Obj_SpawnEnemy).l
