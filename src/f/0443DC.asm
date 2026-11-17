; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn7G - spawns a hall hover pod at a random speed ($66-$CC) on course 7 ($447DE: waypoint gosub $45336 every $3C0 ticks) with attack G ($447EC: turn rate $555, five-shot fans), cruise $1AA on both axes
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_0443E8: score $1F4, energy $1C0/$1C0, accel/cruise $1AA, coroutine 0 $447DE, coroutine 1 $447EC, JitterPos $12C $28, MoveY -$96, Goto the hover pod body $42152 (as Enemy_HoverPod_Spawn1A); the wave spawn entry at $041726 (x -$C, y $A0) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn7G:
	lea.l	(Scr2_0443E8).l,a2
	jmp	(Obj_SpawnEnemy).l
