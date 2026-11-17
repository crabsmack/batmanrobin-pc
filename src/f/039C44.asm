; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnStay10C - spawner of a gift box (box body Scr2_037C02) that stays put, opens after $1E0+$1E0 ticks into a 10-figure chain (ToyChain_MakeBox10C), then hops ($1E/$F0) three times at random $3C0-$780 intervals and once high ($46/$168), repeating
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script
; Evidence: Scr2_039C50: Enemy_GiftBox_SpawnRoamWide10's opening without Op2_AddPopulation (coroutine 0 $039C62: SpawnChild $2A $03A326, StartChildCoroutine3Rel/0Rel, JumpAnim), Goto $037C02; the wave spawn entry at $03490E (x $14A, y $E098) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnStay10C:
	lea.l	(Scr2_039C50).l,a2
	jmp	(Obj_SpawnEnemy).l
