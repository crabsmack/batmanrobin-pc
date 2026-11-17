; RAM symbols, as they are identified (`dkit disasm global ADDR g_Name "comment"`):
;   g_Example = $FFFFC95A	; word: what it is (evidence: Func_Name)
g_VIntOff = $FFFFF6FC	; byte: nonzero = VInt_Handler returns at once (set by scene init $9AAC with the display off, cleared by the frame loop $9BF0)
g_FrameBusy = $FFFFF6FB	; byte: nonzero while the main loop builds a frame (st by $9AAC/$9B86/$9BAC, cleared by $9BF0 before it waits): VInt_Handler then takes VInt_BusyPath (no DMA flush, sound or fade); the V-int also adds 1 around its interruptible tail so a nested V-int takes the short path
g_VIntDone = $FFFFF6FA	; byte: st by VInt_Handler's full path; the frame loop $9BF0 clears it and spins until it is set
g_VIntCount = $FFFFF5F8	; long: V-int counter, +1 on both VInt_Handler paths ($9BF0 waits 2 ticks on it when $F4F0 is set)
g_BgTaskRunning = $FFFFF4BD	; byte: set by the background-task resume $A20C before its fake RTE; VInt_Handler clears it and saves the interrupted SR/PC and registers into g_BgTaskContext
g_BgTaskContext = $FFFFE066	; d0-d2/d6-d7/a0-a4 + SR/PC of the background task (decompressor), saved by $A1F0 and VInt_Handler, restored by $A20C
g_FadeTarget = $FFFFF702	; byte: palette brightness the fade steps toward ($A43C); 0 = normal (cleared by $A9F6, set by $9D24)
g_FadeLevel = $FFFFF703	; byte: current palette brightness offset, stepped by 2 toward g_FadeTarget by $A43C each V-int; +-$E = fully faded ($A9F6, $9AAC compares with $E)
g_FadeLevelVInt = $FFFFF704	; byte: g_FadeLevel latched by VInt_Handler before $A43C steps it; read by the main loop ($9C50, $9D24, $471C0)
g_DMABudget = $FFFF9914	; word: per-frame DMA cost budget, from the scene script in $9AAC; DMA_ResetQueue copies it to g_DMABudgetLeft
g_DMABudgetLeft = $FFFF9916	; word: DMA cost still allowed this frame; DMA_Reserve and Spr_QueueArtDMA subtract from it
g_DMAQueueFree = $FFFF9918	; word: free DMA queue slots (40 after DMA_ResetQueue); DMA_FlushQueue jumps into its unrolled copy at free*8
g_DMAQueuePtr = $FFFF991A	; word: DMA queue write pointer (RAM address); writers store 14-byte entries there and store the pointer back
g_DMAQueue = $FFFF991C	; 40 x 14 bytes: queued DMA commands (VDP regs $93-$97 + 2 command words), sent by DMA_FlushQueue in the V-int
g_SprNodesFree = $FFFFF4AE	; byte: display-list entries left this frame (80 from Spr_ResetList); every Spr_Add* takes one and clamps it at 0 when exhausted
g_SATPtr = $FFFF9B54	; word: write pointer into g_SATBuffer (RAM address); reset by Spr_ResetList, advanced by Spr_BuildSAT, Spr_DrawDirect and direct writers like $F808
g_SATBuffer = $FFFF9B56	; 80 x 8 bytes: sprite attribute table built each frame (y, size|link, tile, x), DMA'd to VRAM $0000 by Spr_QueueSATDMA
g_SprCount = $FFFF9DD6	; word: sprites written to g_SATBuffer this frame (also the next link value); cleared by Spr_ResetList, read by Spr_QueueSATDMA
g_SprCountLo = $FFFF9DD7	; byte: low byte of g_SprCount, stored back by Spr_BuildSAT / Spr_DrawDirect after they add sprites
g_SprOffsetX = $FFFF9DD8	; word: added to every display-list sprite x by Spr_BuildSAT; $93DC sets it to $20 - $DFF0
g_SprOffsetY = $FFFF9DDA	; word: added to every display-list sprite y by Spr_BuildSAT; $93DC sets it to $20 - $DFF4
g_SprNodeAlloc = $FFFF9DDC	; word: display-list node allocator, grows down from $A3A4 (Spr_ResetList) by $12 per Spr_AddSorted/Spr_AddBack node
g_SprFrontEnd = $FFFF9DDE	; word: end of the front list built upward from $9E16 by Spr_AddFront (+$12 per entry); Spr_BuildSAT links it to the sorted list
g_SprSortHead = $FFFF9DE0	; 18-byte head node of the depth-sorted display list (Spr_ResetList links it to the tail sentinel at $9DF2)
g_SprSortFirst = $FFFF9DE6	; word: head node's next link (+6) = the first sorted entry; Spr_BuildSAT starts its walk from it
g_SprSortTail = $FFFF9DF6	; word: the tail sentinel's payload pointer; Spr_AddBack writes the new entry below it and moves it to the new sentinel
g_SprDirectMaxY = $FFFF9B4C	; word: Spr_EmitClipped drops pieces with y >= this ($100 from scene init $9AAC)
g_SprDirectBiasY = $FFFF9B4E	; word: added to piece y by Spr_EmitClipped after clipping ($60 from $9AAC)
g_SprDirectMaxX = $FFFF9B50	; word: Spr_EmitClipped drops pieces with x >= this ($160 from $9AAC)
g_SprDirectBiasX = $FFFF9B52	; word: added to piece x by Spr_EmitClipped after clipping ($60 from $9AAC)
g_ArtRelRingWr = $FFFFA764	; word: write pointer of the art release ring $8F00-$8FFF (descriptors whose last user let go); also the base the RAM frame table is indexed from with negative frame numbers (Spr_ChangeFrame)
g_ArtRelRingWrLo = $FFFFA765	; byte: low byte of g_ArtRelRingWr, advanced by addq.b #2 so the ring wraps in 256 bytes (Spr_SwapFrameArt, Spr_ReleaseFrame, ArtCache_Release)
g_ArtRelRingRd = $FFFFA766	; word: read pointer of the release ring; ArtCache_Alloc evicts from here
g_ArtDescRingWr = $FFFFA768	; word: write pointer of the free-descriptor ring $8E00 (evicted descriptors return here)
g_ArtDescRingRd = $FFFFA76A	; word: read pointer of the free-descriptor ring $8E00 (ArtCache_Alloc takes descriptors here)
g_ArtBlockRingWr = $FFFFA76C	; word: write pointer of the free VRAM block ring $8D00 (ArtCache_AddBlocks, surplus evicted blocks)
g_ArtBlockRingRd = $FFFFA76E	; word: read pointer of the free VRAM block ring $8D00 (ArtCache_Alloc takes blocks here)
g_ArtBlocksFree = $FFFFA770	; byte: 16-tile VRAM blocks available, counting cached (released) art; Spr_SwapFrameArt / ArtCache_Acquire reserve from it
g_ArtBlocksSpare = $FFFFA771	; byte: blocks actually in the free ring (no cached art); ArtCache_Alloc evicts when a request exceeds it
g_ArtDescsFree = $FFFFA772	; byte: art descriptors available, counting cached ones (50 from ArtCache_Init)
g_ArtDescsSpare = $FFFFA773	; byte: descriptors actually in the free-descriptor ring; ArtCache_Alloc evicts one when it is 0
g_ArtNextBlockTile = $FFFFAD50	; word: first tile of the next block ArtCache_AddBlocks will create (+$10 per block)
g_ObjPoolFree = $FFFFF4B0	; byte: free $5A-byte object blocks (140 from ObjPool_Init at scene init); ObjPool_Alloc* and $8912 take, ObjPool_Free gives back
g_ObjPoolHead = $FFFFAD52	; word: head of the object pool's free list (each free block's first word links the next)
g_ObjListHead = $FFFFDE8C	; 8-byte head node of the main object list (+4 prev, +6 next); Obj_RunList walks from it, Obj_InitList ($88C2) links it to g_ObjListTail
g_ObjListTail = $FFFFDE94	; tail sentinel of the main object list; handler = Obj_ListEnd (stored by Obj_InitList $88C2); Obj_LinkAtEnd/Obj_NewMulti/Obj_NewClear/Obj_New insert new objects before it (+4 = last object)
g_ArtBlockRing = $FFFF8D00	; 256-byte ring of free 16-tile VRAM block tile numbers (ArtCache_Init fills it from tile $5F)
g_ArtDescRing = $FFFF8E00	; 256-byte ring of free art descriptor pointers (ArtCache_Init: 50 into g_ArtDescs)
g_ArtRelRing = $FFFF8F00	; 256-byte ring of released art descriptors still holding VRAM (evicted by ArtCache_Alloc)
g_ArtDescs = $FFFFA774	; 50 x $1E bytes: art descriptors (refcount byte, block count, ring flag, handle back pointer, up to 12 block tiles)
g_VM2SavedSP = $FFFFDEC2	; long: stack pointer saved by VM2_RunObject ($7D2) before it runs an object's a2 script; the yield ops ($822, $95C) restore it and rts to the object-list walker; callers that run nested scripts save/restore it ($2604C, $2486)
g_CameraX = $FFFFDFC4	; long 16.16: camera (screen left edge) x in world pixels, high word read; screen x = $3E(obj) - g_CameraX (a2 on-screen tests $82E-$93A: 320-wide screen plus margins); scrolled by $943E, $15916; set by $99D4
g_CameraY = $FFFFDFC8	; long 16.16: camera (screen top edge) y in world pixels, high word read; screen y = $42(obj) - g_CameraY (a2 on-screen tests $882-$93A: 224-tall screen plus margins)
g_ObjPopulation = $FFFFDFAA	; word: sum of the live script objects' population weights (byte $18(obj)); Op2 $A26/$A32 add, $A0C and Obj_Destroy subtract (floored at 0); spawner scripts wait while it is >= an operand ($437A-$43A0) or test it for 0 ($435E); 0..$15 in the TAS
g_TimeScale = $FFFFDEBE	; word 8.8: scales script speed factors ($AF4: $2E(a4) = operand * g_TimeScale >> 8, and the timer tick $30(a4) = that >> 4); $0100 on every sampled TAS frame (no writer found by operand)
g_VM2Arg0 = $FFFFDECE	; long: script argument/scratch register 0 of four ($DECE/$DED2/$DED6/$DEDA); the call-with-arguments ops ($2CE6, $2D00, $3AEA) set its high word from their 4th word operand; ops read it as an angle ($12E4) or a target x ($1364, $16CA); native code uses it as scratch ($3730E, $3768C)
g_VM2Arg1 = $FFFFDED2	; long: script argument/scratch register 1 (see g_VM2Arg0); set from the 3rd word operand of $2CE6/$2D00/$3AEA; read as a target y ($1364, $16CA)
g_StruckBox = $FFFFDEC6	; long: the collision box last found struck (bit 0 of box+$3C was set; cleared) by Obj_TestStruck / $3747C; box+$1A = the attacker's owner id (1/3 score to g_Player1, 2/4 to g_Player2 in Op2_AwardScoreToHitter $A48), box+$20 = damage ($3747C)
g_Player1 = $FFFFF650	; struct ($40 bytes, $F650-$F68F): player 1's record: +0 long score (binary, rises 0 -> $38DCE through the TAS), +$25 bit 0 = in play (Op2_AwardScoreToPlayers $A80)
g_Player2 = $FFFFF690	; struct ($40 bytes, $F690-$F6CF): player 2's record, same layout as g_Player1 (+0 long score, +$25 bit 0 = in play); 0 through the one-player TAS
g_Credits = $FFFFF64E	; word: credits left (continues/joins): set from g_StartCredits at a new game ($11038), -1 when a player joins with Start ($101CE), shown as 'CREDIT nn' by HudOp_ShowCredits; 0 stops joining
g_StartCredits = $FFFFF612	; word: credits at a new game (6, set at boot by $9E02); copied to g_Credits by $11038
g_StartPad = $FFFFF6FF	; byte: pad that pressed Start at the title (1 or 2, $A196 tests the pads' bit 7); 2 makes g_Player2 the record in play at a new game ($11050); cleared by $9E02
g_P1IsRobin = $FFFFF700	; byte: player-select choice, toggled by $49B72 (not.b); nonzero gives g_Player1 the Robin flag (+$25 bit 3) and zero gives it to g_Player2 ($11050); $FF in the robin scenario, 0 in the Batman TAS
g_JoinLocked = $FFFFF4EB	; byte: nonzero = a player out of play cannot join with Start ($101CE); set by HUD_LockJoin (BOSS TIME) and when both players are out (HUD_PlayerOut), cleared by HUD_Init / HUD_UnlockJoin
g_HUDStep = $FFFFED64	; word: next HUD DMA step, an offset into the jump table at HUD_StepIdle run once per frame by HUD_Update (0 idle, 4 load the HUD art, then 8/C/10/14 cycle: P1 ring, P1 panel, P2 ring, P2 panel); HUD_Init sets 4
g_HUDTileBuf = $FFFFEA24	; $240 bytes: tile staging buffer the HUD steps draw into (ring 3x3 tiles, or the panel's 6 columns x 3 tiles: score row + weapon bar or 6x2 text) before DMA to VRAM
g_TerrainFn = $FFFFEA1A	; long: the scene's terrain collision routine for movers with box flag bit 0 clear (Coll_MoveScene); from the scene script ($99D4) or Coll_SetFlatFloor; e.g. Coll_Stage1StreetFloors, Coll_FlatFloor, $82C
g_TerrainFnAlt = $FFFFEA1E	; long: the scene's terrain collision routine for movers with box flag bit 0 set (Coll_MoveScene); set with g_TerrainFn ($99D4, Coll_SetFlatFloor: Coll_FlatFloorRest)
g_FloorY = $FFFFEA22	; word: world y of the flat floor used by Coll_FlatFloor / Coll_FlatFloorRest; set by Coll_SetFlatFloor and scene code ($32F14, $3FFBE, $403F6, $475C2, $47772)
g_PlayMode = $FFFFEA08	; word: scene play mode from the scene script ($99D4): bit 0 on foot, bit 1 the flying shoot-'em-up section of stage 2 (TAS frames 36000-79200; selects the flying shot art/scripts in Weapon_KeepShotArtOne / Weapon_FireFlying); upper bits set by $1048E/$1054A, copied to the player's $24(a4)
g_ShotArtSlots = $FFFFEA0E	; 2 x 3 words (P1 $EA0E, P2 $EA14): the weapon shot frame whose art Weapon_KeepShotArtOne keeps loaded, a release countdown (20 frames) and the previous frame still held
g_FrameCounter = $FFFFE02E	; long: main-loop frame counter, +1 per frame in $9BF0; script timers compare against it ($6EBA, $6F56); Fx_SpawnSkidEffect uses its low 2 bits to spawn every 4th frame
g_SpawnCtx = $FFFFDEE2	; pseudo-object that a4 spawn scripts run on: callers set $3E/$42 x/y velocity, $36/$3A radius/heading speed (ops $55D8/$560E add ctx $36-$42 to the new object's speeds $36-$42; the position comes in d4/d5, op $613A) and $22 attributes, then run the script ($91F2, $915C, $923C, $50CE, $51F2, Weapon_Fire)
g_PlayersInPlay = $FFFFF636	; word: players in play this frame (Player_GatherStats/Player_AddStats count records with +$25 bit 0); the a2 op $39AC picks its 2-player operand when it is 2
g_WeaponLvlMax = $FFFFF638	; word: highest weapon level (+$2A) among players in play, per frame (Player_AddStats); read by the a2 op $3CEE
g_WeaponLvlMin = $FFFFF63A	; word: lowest weapon level (+$2A) among players in play ($7FFF when none), per frame (Player_AddStats)
g_WeaponLvlSum = $FFFFF63C	; word: sum of the weapon levels (+$2A) of players in play, per frame (Player_AddStats)
g_HealthMax = $FFFFF63E	; word: highest health/16 (+$4, negative as 0) among players in play, per frame (Player_AddStats)
g_HealthMin = $FFFFF640	; word: lowest health/16 among players in play ($7FFF when none), per frame (Player_AddStats)
g_HealthSum = $FFFFF642	; word: sum of health/16 of players in play, per frame (Player_AddStats)
g_LivesMax = $FFFFF644	; word: most lives (+$6) among players in play, per frame (Player_AddStats)
g_LivesMin = $FFFFF646	; word: fewest lives (+$6) among players in play ($7FFF when none), per frame (Player_AddStats)
g_LivesSum = $FFFFF648	; word: sum of the lives (+$6) of players in play, per frame (Player_AddStats)
g_PlayerBoxList = $FFFFDEAC	; word: first node of the player-side collision box list (tail sentinel $DEB0, +0 next = 0); $8C1A inits it, $8C3C appends, Hitbox_FindPlayerBox scans it; TAS f3000/f6000/f30000: only boxes owned by the player object $AD54; nodes are pool blocks: +0 next, +2 prev, +$12-$18 rect, +$1A attacker id, +$1E owner energy, +$20 damage taken, +$2E owner
g_EnemyBoxList = $FFFFDEB4	; word: first node of the enemy collision box list (tail sentinel $DEB8, same node layout as g_PlayerBoxList); $8C1A inits it, $8C54 appends, Hitbox_FindEnemyBox scans it and Hitbox_DamageAllEnemies damages every box; TAS f3000: boxes owned by $7D2 script objects with $58 = $A
g_ObjSerial = $FFFFDEC0	; word: next object serial number; Obj_InitSpawned and $101CE copy it to $56(obj) and add 1 (copied into each collision box +4 by $385A)
g_ScreenClearTimer = $FFFFDFBC	; word: frames left of the screen-clear pause; Hitbox_WipeEnemies and $1375E set 15, Camera_UpdateView counts it down; while nonzero the spawn table walker $12E02 and the a4 on-screen tests $6776/$675E fail
g_SceneSprAttr = $FFFFDFBE	; word: default sprite attribute of the scene (from the scene script in $9AAC; $8000 = priority on every sampled TAS frame); Obj_InitSpawned/$101CE/$3B76 copy it to $38(a5), Fx_RunScriptNoParent to g_SpawnCtx+$22
g_CameraLockX = $FFFFDFC0	; long 16.16: when nonzero, Camera_TrackPlayersX centres on this x instead of the players; Camera_LockHere / Camera_LockRightEdge set it, the GO! sign $11A1C and $15730 clear it; Camera_IsLocked tests it
g_CameraSpeedX = $FFFFDFCC	; long 16.16: camera x scroll speed of Camera_TrackPlayersX (eased by $3000 per frame, max 4 px; 0 when the target is behind)
g_CameraSpeedY = $FFFFDFD0	; long 16.16: camera y scroll speed of Camera_TrackPlayersY (clamped by the distance to the top 0 / bottom $A8 limits, max 4 px)
g_OnScreenLeft = $FFFFDFD4	; word: g_CameraX - $18, set by Camera_UpdateView; a4 on-screen tests ($555A, $6776) check x - this < $170
g_OnScreenTop = $FFFFDFD6	; word: g_CameraY - $18, set by Camera_UpdateView; a4 on-screen tests ($555A, $6776) check y - this < $110
g_MoveAreaLeft = $FFFFDFD8	; long 16.16: left edge of the area objects may move in = g_CameraX + g_MoveAreaOfsX (Camera_UpdateView); $4B74 keeps x within it and the width at $DFDC
g_MoveAreaTop = $FFFFDFE0	; long 16.16: top edge of the movement area = g_CameraY + g_MoveAreaOfsY (Camera_UpdateView); $4B74 keeps y within it and the height at $DFE4
g_MoveAreaOfsX = $FFFFDFE8	; long 16.16: movement area left offset from the camera (high word from the scene data in $99D4)
g_MoveAreaOfsY = $FFFFDFEC	; long 16.16: movement area top offset from the camera (high word from the scene data in $99D4)
g_ViewX = $FFFFDFF0	; long 16.16: displayed camera x = g_CameraX (Shake_Update); Camera_UpdateView sets g_SprOffsetX = $20 - it; read by $9848/$9884
g_ViewY = $FFFFDFF4	; long 16.16: displayed camera y = g_CameraY + g_ShakeY (Shake_Update); Camera_UpdateView sets g_SprOffsetY = $20 - it; read by $9848/$9884
g_ScriptTimer = $FFFFDFBA	; word: shared countdown, -1 per frame in Camera_UpdateView down to 0; a4 op $647C adds its operand, op $646E branches while it is 0; $35E8C sets $14
g_HScrollA = $FFFFDFB2	; word: plane A horizontal scroll, written with g_HScrollA+2 (plane B) as one long to VRAM $BC00 by the V-int raster setups ($AF10); Scroll_SetPlaneA and the stage code ($48C36, $4A1C8) set it
g_VScrollA = $FFFFDFB6	; word: plane A vertical scroll, written with the plane B word ($DFB8) as one long to VSRAM 0 ($AF10/$AF2E/$AF42); Scroll_SetPlaneA and stage code ($48998, $4A1C8) set it
g_WaveCount = $FFFFDFAC	; word: live wave-director objects (a5-VM scripts): Wave_Start adds 1, their end $6A76 subtracts 1 (floored at 0)
g_WaveGen = $FFFFDFAE	; word: current wave generation: a wave object whose serial ($A) is below it ends on its next frame ($6A14); Wave_EndAll and a5 op $6EEC raise it to g_WaveSerial
g_WaveSerial = $FFFFDFB0	; word: last wave serial handed out; Wave_Start stamps each new wave object with it +1
g_ArcTargetX = $FFFFDF96	; word: x of the point Arc_PlanToMark aims at; a2 op $5378 stores the linked object ($10(a4)) x there
g_ArcTargetY = $FFFFDF9A	; word: y of the point Arc_PlanToMark aims at; a2 op $5378 stores the linked object y there
g_ShakeY = $FFFFE016	; long 16.16: vertical screen-shake offset, >= 0 (bounces off 0 with 3/4 damping in Shake_Update); added to g_ViewY; 0 = idle (the Shake_*IfIdle starters test it)
g_ShakeVel = $FFFFE01A	; long 16.16: screen-shake upward speed; the Shake_Start* entries set 3..12 px; Shake_Update subtracts g_ShakeGravity each frame
g_ShakeGravity = $FFFFE01E	; long 16.16: per-frame pull on g_ShakeVel (1.0-3.0 from the Shake_Start* entries)
g_Shake2Pos = $FFFFE022	; long 16.16: second damped oscillator of Shake_Update (pulled to 0 by g_Shake2Pull, speed g_Shake2Vel); Shake_Start kicks it when its high word <= 12; no reader outside the shake code was found
g_Shake2Vel = $FFFFE026	; long 16.16: speed of g_Shake2Pos (Shake_Start sets the high word to 4)
g_Shake2Pull = $FFFFE02A	; long 16.16: per-frame pull of g_Shake2Vel toward g_Shake2Pos = 0 ($2000 from Shake_Start)
g_FlashReq = $FFFFF4C0	; byte: nonzero = $A7F0 fills CRAM with white ($0EEE) once and clears it; set by Pal_FlashWhite, a4 op $6492 and $AA9C
g_FlashTimer = $FFFFF4C1	; byte: frames the white flash lasts; $A7F0 counts it down and restores the palette at 0 (4 from Pal_FlashWhite, 2 from $AA9C)
g_LayerA = $FFFFED9C	; struct: plane A map scroller state: +0/+2 fine x/y (low 4 bits), +$32/+$34 tile column/row; Scroll_SetPlaneA turns it into g_HScrollA/g_VScrollA; $7186/$737C/$73EE use it
g_GoSignKeepLock = $FFFFF4EC	; byte: nonzero = the GO! sign leaves g_CameraLockX alone when it stops (set by HUD_StartGoSignKeepLock, cleared by HUD_StartGoSign, read by HUD_UpdateGoSign)
g_GoSignFrame = $FFFFED72	; word: the GO! sign's current sprite frame (Spr_ChangeFrame result), released by HUD_UpdateGoSign when the sign has left
g_GoSignAnim = $FFFFED74	; word: GO! sign animation counter 0-31, frame = counter/4 (HUD_UpdateGoSign)
g_GoSignSpeed = $FFFFED76	; long: GO! sign x speed 16.16 (-4.0 entering, braked by $1A00 a frame; +$1A00 rising to 4.0 leaving) (HUD_StartGoSign, HUD_GoSignLeave, HUD_UpdateGoSign)
g_GoSignX = $FFFFED7A	; long: GO! sign x 16.16, starts at $168; drawn at x+$20, y $90 (HUD_UpdateGoSign)
g_GoSignState = $FFFFED7E	; word: GO! sign state: 0 off, -1 moving, >0 hold timer counted down by 2 ($47 from HUD_GoSignLeave)
g_GoSignCameraX = $FFFFED80	; word: g_CameraX when the GO! sign started; the sign leaves once the camera scrolls past it (HUD_UpdateGoSign)
g_BossMeterArt = $FFFFED82	; word: ArtCache handle of the boss meter's 16-tile VRAM block (BossMeter_Init acquires, BossMeter_Free releases)
g_BossMeterVRAM = $FFFFED84	; long: VDP write command for the boss meter's VRAM block (BossMeter_Init, used by BossMeter_QueueTileDMA)
g_BossMeterTile = $FFFFED88	; word: first tile of the boss meter's VRAM block, the SAT tile field (BossMeter_Init, BossMeter_DrawSprite)
g_BossMeterObj = $FFFFED8A	; word: the boss energy meter object, 0 = none (BossMeter_Spawn sets, BossMeter_Free clears, BossMeter_Exists/Show/Defeated read)
g_BossEnergyMax = $FFFFED8C	; word: the boss's full energy, 100% of the meter (BossMeter_Show = $52 of the boss, BossMeter_AddPart adds; $2200 for the stage 1 balloon, TAS f4560)
g_BossEnergy = $FFFFED8E	; word: the boss's current energy shown by the meter (BossMeter_Show/AddPart, a2 ops $3A4A/$3A54 update it from $52 of the boss, BossMeter_Defeated zeroes it; $2200 -> 0 over TAS 4560-6000)
g_BossMeterTiles = $FFFFEC64	; struct ($200 bytes, 16 tiles): the boss meter's tile buffer: ring frame tiles (BossMeter_AnimRing) with the percentage digits masked in at +$50/+$90, DMA'd by BossMeter_QueueTileDMA
g_CollStepX = $FFFFED90	; long: Coll_MoveBlockMap's x step routine (right or left), resumed after a vertical step
g_CollStepY = $FFFFED94	; long: Coll_MoveBlockMap's y step routine (down or up), resumed after a horizontal step
g_CollBlockAttr = $FFFFED98	; word: attribute word of the block-map block last entered (block table entry +2), returned in d3 by Coll_BlockMapResult
g_CollMoveDX = $FFFFED9A	; word: Coll_MoveBlockMap's signed x move, picks Coll_WalkBlockMapRight/Left to finish after a vertical stop
g_PlaneAVRAM = $FFFFEE34	; word: plane A name table VRAM address ($C000), set by Layer_SetPlanes64x64/128x32; the scene op $71CC copies it to +$24 of g_LayerA
g_PlaneBVRAM = $FFFFEE36	; word: plane B name table VRAM address ($E000), set by Layer_SetPlanes64x64/128x32; read by the scene op $71DA, $03CEF8, $049212
g_ScreenBlocksX = $FFFFEE38	; word: block columns a full-screen redraw covers ($16 = 22 x 16 px), +$3E of a layer (Layer_Init, Layer_DrawScreen)
g_ScreenBlocksY = $FFFFEE3A	; word: blocks a queued column covers ($10 = 16 x 16 px), +$40 of a layer (Layer_Init, Layer_QueueColumn)
g_PlaneCellsX = $FFFFEE3C	; word: VDP plane width in cells ($40 or $80) (Layer_SetPlanes*; Layer_Init +$3A; the scene ops $7256/$74A8)
g_PlaneCellsY = $FFFFEE3E	; word: VDP plane height in cells ($40 or $20) (Layer_SetPlanes*; Layer_Init +$3C; the scene ops $7256/$74A8)
g_PlaneRowBytes = $FFFFEE40	; word: bytes per plane row ($80 or $100) (Layer_SetPlanes*; Layer_Init +$36; scene ops $7256/$72D2/$74A8, $0144D4)
g_PlaneBytes = $FFFFEE42	; word: bytes per plane ($2000) (Layer_SetPlanes*; Layer_Init +$38, the wrap mask of Layer_QueueColumn)
g_TileQueuePtr = $FFFFEE44	; long: write pointer into g_TileQueue (Layer_QueueColumn/Layer_ScrollStepY append, Layer_FlushTileQueue/Layer_ResetTileQueue rewind)
g_TileQueue = $FFFFEE48	; struct: queue of plane cell writes: per entry word -(8 x count), VDP command long, then control word + data long per cell pair; 0 = end (Layer_QueueColumn, Layer_FlushTileQueue)
g_TriggerList = $FFFFF048	; struct: the loaded trigger table sorted by key: longs (key word, entry offset word), $FFFF sentinel (Trigger_LoadX/LoadY, Trigger_RunX/RunY)
g_TriggerNext = $FFFFF368	; long: next g_TriggerList element to test (Trigger_LoadX/LoadY, Trigger_RunX/RunY)
g_TriggerTable = $FFFFF36C	; long: the loaded trigger table's entries (after its count word); g_TriggerList offsets are relative to it (Trigger_Load*, Trigger_Fire)
g_FoundBox = $FFFFDECA	; long: the collision box found by the a2 overlap ops $375A/$3768 (Hitbox_FindPlayerBox/FindEnemyBox); read by $10742 (damage it) and Item_FoundBoxPlayer (who collected a pickup)
g_DropCooldown = $FFFFF370	; word: random cooldown of the random health drops (Drop_RandomHealthB, the unrun action 48 at $12FE4): no drop while it counts down, re-armed from $995A
g_ScrollMode = $FFFFE55A	; word: V-int scroll-upload mode, an offset into the jump tables at $AE7C (full path) / $AEC4 (busy path); $C = per-line H-scroll of both planes from g_HScrollLinesA/B + VSRAM (Scroll_RunParallax); scenes set 4/8/$28/$2C/$38/$3C ($AE44, $1FECC, $3BB88)
g_HScrollLinesA = $FFFFE1D2	; 224 words: plane A H-scroll per screen line, DMA'd to VRAM $BC00 with stride 4 ($AFFC, mode $C); filled by Scroll_FillBands via ScrollCmd_LayerA/CameraA and stage code ($15D90, $334D2, $3BB88)
g_HScrollLinesB = $FFFFE392	; 224 words: plane B H-scroll per screen line, DMA'd to VRAM $BC02 with stride 4 ($AFFC); filled by ScrollCmd_CameraB/CameraOfsB and stage code ($15D90, $24660, $33448)
g_VScrollB = $FFFFDFB8	; word: plane B vertical scroll, sent with g_VScrollA as one long to VSRAM 0; set by ScrollCmd_CameraB (= g_ViewY) and section loops ($1FB3A, $33448, $3BB88)
g_RandState = $FFFFF5FC	; word: RNG state word, mixed with g_RandState2 by Rand_Next every call; $79 at boot (Main_Start)
g_RandState2 = $FFFFF5FE	; long: RNG state long (Rand_Next rol/eor/swap); $004B00D3 at boot (Main_Start writes the words $4B and $D3 at $F5FE/$F600)
g_SectionId = $FFFFF602	; word: current game section (hi byte stage 1-4; $0001-$0004 stage openings, $1100-$4401 gameplay sections), advanced by Section_RunNext through Tbl_009FFC, dispatched by Tbl_00A0AC; 0 = not in a stage (Pause_Update off); set from scene headers (Scene_InitCommon) and the a6 op $772E, tested by the a2 op $42FE
g_FrameCallback = $FFFFF604	; long: routine Frame_End calls once per frame after the V-int: SceneCb_None ($9CE6, set by Scene_InitCommon), SceneCb_CheckGameOver, SceneCb_EndTimer, SceneCb_WaitFadeExit, SceneCb_WaitPlayersExit ($1054A); its high word is also written/compared as a word ($9C50, $1569C, $11208)
g_SceneResult = $FFFFF608	; word: how the scene ended: 0 = continue with the next one (Scene_RunList), 1 = Start / continue chosen ($47C6A, title), 2 = game over (SceneCb_CheckGameOver); set by Scene_EndAfterDelay*, cleared by Scene_InitCommon; read by Main_Start
g_SceneTimer = $FFFFE034	; word: scene countdown in frames: the end delay of Scene_StartEndTimer / SceneCb_EndTimer, and the title idle timeout ($499B6 $1F4, $499E8 $7080)
g_ExitFadeTarget = $FFFFF4B8	; byte: fade target SceneCb_WaitFadeExit holds and waits for (g_FadeLevelVInt == it); saved by Scene_FadeOutExit / SceneCb_EndTimerWhite after $AA2A/$AA4C
g_BgTaskActive = $FFFFF4BC	; byte: nonzero while a background decompression task exists: set by BgTask_Start, cleared by the decompressor $A23E when it finishes; BgTask_Resume does nothing when 0, the a6 op $713E waits for it to clear
g_BufPtrs = $FFFFE092	; 16 longs: RAM buffer pointers of the a6 scene scripts (word operand = byte offset), decompression destinations ($7108/$7120) and sprite-bank sources ($70E6); all $FF0000 after Scene_ResetBufPtrs; $7148 stores $E08E into one
g_MusicCur = $FFFFF622	; word: song last started by Sound_PlayMusic / Sound_RestartMusic (0 = silence)
g_MusicLock = $FFFFF6FE	; byte: nonzero = Sound_PlayMusic ignores requests; set by Main_Attract / Main_Title around the attract and title scenes (so their demo stages keep the title music), cleared by Main_NewGame
g_IntroNoSkip = $FFFFF701	; byte: nonzero = Start does not skip the intro scene loop $47C6A; set by Main_Start for the first attract pass, cleared by $48AD2 and after the list (Main_Attract)
g_DebugPause = $FFFFF60E	; word: nonzero = Pause_Update enters the debug pause menu $1569C (A slow motion, B free camera, C end the section) instead of the skip code; 0 at boot (Main_Start), no other writer found
g_SlowMotion = $FFFFF4F0	; byte: nonzero = Frame_End waits 2 extra V-ints per frame; toggled by A in the debug pause menu $1569C
g_FreeCam = $FFFFF4F1	; byte: nonzero = Frame_RunWorld/Frame_RunWorldLayerA run the pad-driven free camera $15730 instead of the objects; toggled by B in the debug pause menu $1569C; tested by $32F14/$33274
g_FrameParity = $FFFFF4B6	; byte: inverted every frame by Frame_End (odd/even frame); read by 9 routines ($4454, $65B4, $6952, $F96A, $15D90 ...) to do work on alternate frames
g_WaveTimerStep = $FFFFDEBC	; long 16.16: amount a wave director subtracts from its spawn timer $2E(a6) each frame ($6A14); $100 after Scene_InitCommon
g_SceneShadowHL = $FFFFF4C2	; byte: scene header flag (Scene_InitCommon): nonzero = $A7F0 puts VDP reg 12 back to $89 (H40 + shadow/highlight) when the white flash ends
g_SceneFixedSpr = $FFFFF4E8	; byte: scene header flag (Scene_InitCommon): nonzero = $F808 DMAs $100 bytes to VRAM $BB00 once and puts 7 sprites (tile $5D8, x $80, 32 px apart) at the head of the SAT every frame (Frame_Begin*)
g_MoveAreaW = $FFFFDFDC	; word: width of the movement area (right - left offset from the section header, Scene_InitStage); $4B74 keeps objects within g_MoveAreaLeft + it
g_MoveAreaH = $FFFFDFE4	; word: height of the movement area (bottom - top from the section header, Scene_InitStage); $4B74 keeps objects within g_MoveAreaTop + it
g_JoinOfsX = $FFFFEA0A	; word: x offset from the camera where a joining player appears ($101CE: camera + it when the player was not in play at the section start); from the section header (Scene_InitStage)
g_JoinOfsY = $FFFFEA0C	; word: y offset from the camera where a joining player appears ($101CE); from the section header (Scene_InitStage)
g_ExitTimer = $FFFFEA06	; word: frames SceneCb_WaitPlayersExit counts (only on frames where every player in play has bit 12 of script variable 1) before the section ends; $82 ($10528) or a script operand ($10530)
g_StartLives = $FFFFF618	; word: lives a player starts with (2, Main_Start); $101A8 copies it to record +6
g_StartWeaponType = $FFFFF614	; word: weapon type a player starts with (0, Main_Start); $101A8 copies it to record +$28
g_StartWeaponLvl = $FFFFF616	; word: weapon level a player starts with (0, Main_Start); $101A8 copies it to record +$2A
g_StartEnemyScale = $FFFFF61E	; word: enemy energy scale for a new game ($100 = x1.0, Main_Start); Player_NewGameSetup copies it to $F64A, which the a2 op $39AC multiplies enemy energy by (>> 8)
g_StartBossScale = $FFFFF620	; word: boss energy scale for a new game ($100, Main_Start); Player_NewGameSetup copies it to $F64C, which $39AC uses for owner class $B
g_Pad1 = $FFFFF624	; 4 bytes: pad 1 as the game sees it, read by $A3AE on the V-int's full path: +0 held, +1 previous, +2 newly pressed, +3 newly released (bit 7 Start, 6 A, 5 C, 4 B, 3-0 R L D U); $A3AE then copies it to g_PadLive1; joining ($101CE), Pad_CheckStart and menus ($496EC, $499E8) read +2
g_Pad2 = $FFFFF628	; 4 bytes: pad 2, same layout as g_Pad1 ($A3AE, port $A10005); copied to g_PadLive2
g_PadLive1 = $FFFFF62C	; 4 bytes: pad 1 record (g_Pad1 layout) refreshed on every V-int: copied from g_Pad1 on the full path, read directly by $A3D4 on the busy path, so Pause_Update can poll it while the main loop is held
g_PadLive2 = $FFFFF630	; 4 bytes: pad 2 record refreshed on every V-int (see g_PadLive1); polled by Pause_Update
g_UnkStart_F610 = $FFFFF610	; word: new-game setting, 1 at boot (Main_Start), copied to $F634 by Player_NewGameSetup; no reader of $F634 found (meaning unknown)
g_VM2Arg2 = $FFFFDED6	; long: script argument/scratch register 2 (see g_VM2Arg0); high word set from the 2nd word operand of Op2_GosubArgs/GosubAbsArgs ($2CE6/$2D00) and $3AEA; read by $3490 (player window y), $28794 (into var 1); native scratch at $3768C
g_VM2Arg3 = $FFFFDEDA	; long: script argument/scratch register 3 (see g_VM2Arg0); high word set from the 1st word operand of Op2_GosubArgs/GosubAbsArgs ($2CE6/$2D00) and $3AEA; read by $3490 (player window y); native scratch at $3768C
g_EnemyEnergyScale = $FFFFF64A	; word: energy scale for enemies this game ($100 = x1.0), copied from g_StartEnemyScale by Player_NewGameSetup; the a2 op $39AC sets an enemy energy $52(a6) = operand * scale >> 8
g_BossEnergyScale = $FFFFF64C	; word: energy scale for bosses (owner class $B) this game ($100 = x1.0), copied from g_StartBossScale by Player_NewGameSetup; the a2 op $39AC uses it instead of g_EnemyEnergyScale
g_TargetTieFlip = $FFFFF4EA	; byte: flipped (not.b) by the a2 player-target picker ($2DA2) when both players are targetable and equally targeted (record +$36): 0 after the flip picks g_Player1, nonzero g_Player2, so ties alternate
g_WeaponItemCount = $FFFFF372	; word: weapon and power-up pickups alive; +1 in Item_WeaponItemInit, -1 (floor 0) in Item_WeaponItemGone; WeaponDrop_ItemsOut makes the drop director wait while nonzero
g_WeaponDropper = $FFFFF374	; word: the weapon-drop director object (a6 address) or 0; set by WeaponDrop_Command when it spawns it (Scr2_013F2A), cleared by WeaponDrop_Stop
g_TextGlyphFrames = $FFFFF376	; words x 28 (F376-F3AD): the sprite frame held per text glyph (A-Z slots 0-50, ! 52, ? 54; Tbl_0141B2), swapped by Spr_ChangeFrame in Text_DrawSprites so each letter's art loads once
g_PalSource = $FFFFE0D2	; 128 bytes: the scene's palette, 4 lines x 16 CRAM words, before fading; loaded per line by Pal_LoadLine0-3 ($ABCA-$AC1E), read by Fade_Step to build g_PalFaded
g_PalFaded = $FFFFE152	; 128 bytes: the palette after fading (g_PalSource + g_FadeLevel + per-line tint, clamped), rebuilt by Fade_Step; Pal_VIntUpload DMAs its changed lines to CRAM (descriptors in Tbl_00A87E, source $FFE152); Fade_Step forces word 0 (backdrop) to black
g_FadeDelay = $FFFFF4BE	; byte: frames between fade steps minus 1, the reload of g_FadeDelayCount (Fade_Step); 2 from Fade_StartIn/ToNormal/StartOut*, SceneCb_WaitFadeExit; 5/6/2 from the Fade_FlashWhite* starts; 9 from $493D2
g_FadeDelayCount = $FFFFF4BF	; byte: fade step countdown: Fade_Step decrements it and moves g_FadeLevel 2 toward g_FadeTarget when it goes negative, then reloads g_FadeDelay; the fade starts clear it so the first step is immediate
g_LzOutEnd = $FFFFE08E	; long: end of the output of the last LZ decompression (Lz_Decompress stores a1 when the end code is read); the a6 op $7148 copies it into a g_BufPtrs slot
g_PalLineCache = $FFFFF4D7	; 4 x 4 bytes: per palette line the r/g/b offsets Fade_Step last applied, then a changed flag (+3: $F4DA/$F4DE/$F4E2/$F4E6, $FF = rebuilt this V-int); Pal_VIntUpload DMAs the lines whose flag is set; Fade_MaxTint/MinTint scan the tint records below it
g_TintDelay = $FFFFF4C3	; byte: frames between tint steps minus 1 (reload of g_TintDelayCount, Tint_Step); 0/2/1/1/5 from Tint_FlashC1/C3/E2/A2/E6
g_TintDelayCount = $FFFFF4C4	; byte: tint step countdown: Tint_Step decrements it and moves g_TintLevel 2 toward g_TintTarget when it goes negative, then reloads g_TintDelay
g_TintTarget = $FFFFF4C5	; byte: level g_TintLevel steps toward (Tint_Step); the Tint_Flash* starts clear it (flash decays to 0)
g_TintLevel = $FFFFF4C6	; byte: current tint level (brightness offset, +$A..+$E at a flash start), stepped by Tint_Step; Tint_StepLine0/1/3 copy it into a line's r/g/b tint, and $48E80 also uses its negation for the other lines
g_Pal0TintR = $FFFFF4C7	; byte: palette line 0 red brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal0TintG = $FFFFF4C8	; byte: palette line 0 green brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal0TintB = $FFFFF4C9	; byte: palette line 0 blue brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal0Reload = $FFFFF4CA	; byte: nonzero = palette line 0 was reloaded into g_PalSource (Pal_LoadLine0 and stage code set it): Fade_Step rebuilds the line even if its levels are unchanged, then clears it
g_Pal1TintR = $FFFFF4CB	; byte: palette line 1 red brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal1TintG = $FFFFF4CC	; byte: palette line 1 green brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal1TintB = $FFFFF4CD	; byte: palette line 1 blue brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal1Reload = $FFFFF4CE	; byte: nonzero = palette line 1 was reloaded into g_PalSource (Pal_LoadLine1 and stage code set it): Fade_Step rebuilds the line even if its levels are unchanged, then clears it
g_Pal2TintR = $FFFFF4CF	; byte: palette line 2 red brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal2TintG = $FFFFF4D0	; byte: palette line 2 green brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal2TintB = $FFFFF4D1	; byte: palette line 2 blue brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal2Reload = $FFFFF4D2	; byte: nonzero = palette line 2 was reloaded into g_PalSource (Pal_LoadLine2 and stage code set it): Fade_Step rebuilds the line even if its levels are unchanged, then clears it
g_Pal3TintR = $FFFFF4D3	; byte: palette line 3 red brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal3TintG = $FFFFF4D4	; byte: palette line 3 green brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal3TintB = $FFFFF4D5	; byte: palette line 3 blue brightness offset (signed, added to g_FadeLevel for that channel by Fade_Step); set by Tint_Step* and stage code; scanned by Fade_MaxTint/MinTint
g_Pal3Reload = $FFFFF4D6	; byte: nonzero = palette line 3 was reloaded into g_PalSource (Pal_LoadLine3 and stage code set it): Fade_Step rebuilds the line even if its levels are unchanged, then clears it
g_Pal0Changed = $FFFFF4DA	; byte: $FF = Fade_Step rebuilt palette line 0 of g_PalFaded this V-int (byte +3 of its g_PalLineCache record), 0 = unchanged; Pal_VIntUpload DMAs the lines whose flag is set
g_Pal1Changed = $FFFFF4DE	; byte: $FF = Fade_Step rebuilt palette line 1 of g_PalFaded this V-int (byte +3 of its g_PalLineCache record), 0 = unchanged; Pal_VIntUpload DMAs the lines whose flag is set
g_Pal2Changed = $FFFFF4E2	; byte: $FF = Fade_Step rebuilt palette line 2 of g_PalFaded this V-int (byte +3 of its g_PalLineCache record), 0 = unchanged; Pal_VIntUpload DMAs the lines whose flag is set
g_Pal3Changed = $FFFFF4E6	; byte: $FF = Fade_Step rebuilt palette line 3 of g_PalFaded this V-int (byte +3 of its g_PalLineCache record), 0 = unchanged; Pal_VIntUpload DMAs the lines whose flag is set
g_PalSource1 = $FFFFE0F2	; 32 bytes: palette line 1 of g_PalSource (16 CRAM words before fading), loaded by Pal_LoadLine1
g_PalSource2 = $FFFFE112	; 32 bytes: palette line 2 of g_PalSource, loaded by Pal_LoadLine2
g_PalSource3 = $FFFFE132	; 32 bytes: palette line 3 of g_PalSource, loaded by Pal_LoadLine3
g_TextMapFont = $FFFFF3AE	; long: font map TextMap_Run reads (word per ASCII code from $20 = tile offset), set to Tbl_0586F2 by TextMap_LoadFont
g_TextMapTileBase = $FFFFF3B2	; word: tile base TextMap_Run adds to every font map entry (priority/palette bits + first font tile, e.g. $E1C9); set by TextMap_LoadFont
g_MenuCursor = $FFFFF3B4	; word: selected menu item; Menu_MoveCursor moves it with Up/Down, TextMap_CmdMenuItem highlights it, the title/options scenes ($496EC, $499B6, $499E8) clear and read it
g_TextMapTypeDst = $FFFFF3B6	; long: typewriter mode: the plane-image write pointer saved by TextMap_PutChars after each character, restored by TextMap_ResumeTyping
g_TextMapTypeAcc = $FFFFF3BA	; word: typewriter accumulator: TextMap_ResumeTyping adds g_TextMapTypeSpeed, a carry writes the next character; set by TextMap_CmdTypewriter
g_TextMapTypeSpeed = $FFFFF3BC	; word: typewriter speed (added to g_TextMapTypeAcc per TextMap_Run call), the operand of TextMap_CmdTypewriter
g_TextMapTypeCount = $FFFFF3BE	; word: typewriter mode: characters left in the current text run (dbra count) saved by TextMap_PutChars
g_TextMapTypeSrc = $FFFFF3C0	; long: typewriter mode: the command-list pointer (next character) saved by TextMap_PutChars, resumed by TextMap_ResumeTyping
g_TextMapTyping = $FFFFF4EE	; byte: nonzero = TextMap_Run is in typewriter mode (one character per carry of g_TextMapTypeAcc); set by TextMap_CmdTypewriter, cleared by TextMap_CmdEnd; the script natives $4A5E8 / $4A734 call TextMap_Run every frame while it is set
g_SceneWorkBuf = $FFFF9000	; RAM work buffer whose layout depends on the scene: the 40x28-word plane image of TextMap_Run / TextMap_Upload (copyright notice, title menu, options, ending texts), line tables ($3BB88), copies ($244A8, $24576), flags ($3CEF8); the H-int raster line tables $FF9040/$FF9240 lie inside it
g_A6ScriptPc = $FFFFE036	; long: pc of the global a6-VM loader script (decompress/DMA jobs): set by Op2_StartA6Script ($43E2) and $7094, saved by the a6 ops ($7202, $7256, $737C, $7416, $74A8) when they park, reloaded by $70A8
g_A6ResumeHook = $FFFFE03A	; long: code that Frame_End runs each frame through $7086 while non-zero (0 = none): $70A8 resumes the a6 loader script at g_A6ScriptPc, a6 wait ops install their own ($713E, $72D2, $743A ...); set by Op2_StartA6Script
g_A6ScriptBusy = $FFFFF4BA	; byte: $FF while the global a6 loader script runs (st by Op2_StartA6Script and $70A8, sf by the a6 end op $70B8); Op2_WaitA6Script* wait for it to clear
g_DebugPadPtr = $FFFFF418	; word: address of the pad record that opened the debug pause menu (Debug_PauseMenu stores a0); Debug_FreeCamStep reads the free camera's input from it
g_DebugCursorX = $FFFFF41A	; long 16.16: free-camera cursor x (world); centred by Debug_CentreCursor, moved by Debug_FreeCamStep, copied to the players by Debug_PlacePlayerAtCursor
g_DebugCursorY = $FFFFF41E	; long 16.16: free-camera cursor y (world); see g_DebugCursorX
g_DebugCursorSpeed = $FFFFF422	; long 16.16: free-camera cursor speed: 0 with no direction held, +$2000 per frame up to 4.0 (Debug_FreeCamStep)
g_LiftVelY = $FFFFE006	; long 16.16: vertical speed of the zeppelin loading-dock lift section (TAS f21729-f29582, 'ZEPPELIN LOADING DOCK' sign at f22200): $240F6 starts it at -$2000 with the goal $DFFE = -1.0, $2410A ramps it by $300 a run and adds it to $E00E; $33BD8 places objects with it; Op2_SubLiftVelY ($4D9C) subtracts it from an object's y move (lift reading inferred)
g_HIntTblFront = $FFFFE552	; word: low word of the RAM address of the line table the H-int raster is reading this frame ($9240 after HInt_Install); HInt_SwapRearm exchanges it with g_HIntTblBack and copies it into the handler's operand, HInt_Rearm re-uses it ($47A72 also reads it)
g_HIntTblBack = $FFFFE554	; word: low word of the RAM address of the line table the game fills for the next frame ($9040 after HInt_Install; 224 words, one VSRAM value per line); the span renderers and stage code load it with movea.w ($24900, $334D2, $3BB88, $4062A, $40980, $471C0); swapped in by HInt_SwapRearm
g_HIntVSRAMCmd = $FFFFE556	; long: VDP command HInt_Arm sends before the raster starts, so the H-int's writes go to VSRAM 0 (plane A, $40000010, HInt_InstallVScrollA) or VSRAM 2 (plane B, $40020010, HInt_InstallVScrollB)
g_ScrollVIntFn = $FFFFE55C	; long: scene routine VInt_ScrollCustom jumps to in scroll mode 4 (set with the mode by $3BB88: $3BBAC, $3BC56, $3BCEC)
g_HIntCode = $FFFFE560	; code: the RAM H-int handler (IRQ4 vector $FFE560): HInt_Install copies the per-line VSRAM writer from Tbl_HIntHandlerSrc ($4C bytes); $1C3F2, $244A8 and $24576 instead write a jmp stub here (move.w #$4EF9 + target long in g_HIntOperand)
g_HIntOperand = $FFFFE562	; word (long for the jmp stubs): the operand of the H-int handler's first instruction: the source address of move.w (xxxx).w,$C00000, i.e. the next line value in the front table; the handler steps its low byte by 2 and its high byte on carry; HInt_Rearm/HInt_SwapRearm reset it; the jmp stubs ($1C3F2, $244A8, $24576) store their target long here
g_HIntBcsDisp = $FFFFE56D	; byte: displacement of the H-int handler's bcs (self-modified): 2 = step the table address's high byte on a low-byte carry; the handler sets $E when it does, so the next carry branches to its tail that writes VDP reg 0 = $04 (H-int off) after 224 lines; HInt_Arm restores 2
g_LeftMaskArtSent = $FFFFF4E9	; byte: set by Spr_MaskLeftColumn once the mask column's art (Art_00F86A, 8 solid colour-6 tiles) is queued for DMA to VRAM $BB00 (tile $5D8); cleared with RAM at scene start (inferred: no other writer)
g_Stage1FireState = $FFFFF518	; byte: stage 1 street plane-A effect: 0 = sky wave (Stage1_WaveSkyLines), 2 = fire started (Stage1_StartFireAt; Stage1_VIntFireStep blanks plane A and sets 1), 1 = fire animating/swaying, $FF = off (Boss_Harley_StopStreetFx at the mid-boss)
g_Stage1SegOfs = $FFFFF50A	; long 16.16: minus the facade x offset inside the current $168-px stage 1 street segment (Stage1_UpdateStreetScroll); the shear input of Stage1_ScrollFacadeLines
g_Stage1Segment = $FFFFF50E	; word: index of the $168-px facade segment at the stage 1 street's view (Stage1_UpdateStreetScroll; shifts the sign slots when it grows); $20866 locates a blast's sign slot from it
g_Stage1FireX = $FFFFF512	; long 16.16: facade x of the stage 1 building fire, snapped to a window by Stage1_StartFireAt; Stage1_SwayFireLines scrolls plane A to it
g_Stage1FirePhase = $FFFFF510	; word: sine phase of the stage 1 fire sway, +2 per frame (Stage1_SwayFireLines)
g_Stage1SkyPhase = $FFFFF516	; word: sine phase of the stage 1 sky ripple, +2 per frame (Stage1_WaveSkyLines)
g_TriX0 = $FFFFE5F8	; word: span triangle vertex 0 x (screen, after the caller subtracts g_ViewX); Span_DrawTriangle(Dither) sort the three vertices by y; set by Boss_BeamShip_SetShipVertex ($29566: the ship, the beam source), $334D2, $471C0
g_TriY0 = $FFFFE5FA	; word: span triangle vertex 0 y (screen); see g_TriX0
g_TriX1 = $FFFFE5FC	; word: span triangle vertex 1 x; set by Boss_BeamShip_SetSpotVertex ($2951E: the beam spot), $334D2, $471C0
g_TriY1 = $FFFFE5FE	; word: span triangle vertex 1 y
g_TriX2 = $FFFFE600	; word: span triangle vertex 2 x; set by $334D2, $471C0
g_TriY2 = $FFFFE602	; word: span triangle vertex 2 y
g_SpanLinesLeft = $FFFFE604	; word: screen lines (of 224) Span_DrawTriangle(Dither) still have to write; caps each part's line count and sets the dither phase
g_CircleSpans = $FFFFE606	; words: circle half-width table, r+1 entries from the top row to the centre row, built by Span_BuildCircle and read by Span_DrawDiscDither (forward for the upper half, backward for the lower)
g_SceneVars = $FFFFF4F4	; per-scene scratch variables $F4F4-$F508, written as g_SceneVars+offset; each scene gives them its own meaning (stage 1 street: +0 fire frame entry, +4 fire x offset, +6..+9 wanted sign states, +$A..+$D drawn states, +$E sign map buffer, +$12 facade x; other users $1C3F2, $2417E, $24660, $3BB88, $3CC82, $47674 ...; the block runs on past $F508 in some scenes: stage 2 arena +$A/+$32/+$5A/+$82 = 20-word column V-scroll tables of plane A, plane B above and below the split, and the H-int's copy of the last; flying section +$12..+$91 = 32 corridor band longs (Flight_UpdateCorridor)); do not give scene-specific global names to these addresses
g_SceneVars2 = $FFFFF59E	; second per-scene scratch block $F59E-$F5DD, written as g_SceneVars2+offset; meanings differ by scene (stage 2 boss arena, Stage2_UpdateScroll/ArenaDrawTilt: +0/+4/+8 pivot x target/speed/position (16.16; floor y = $C4 - (x - pivot) * slope), +$C floor y at the pivot, +$E plane A column V-scroll base (rolls by +$12 while the floor turns), +$12 its roll speed (up to 3.0), +$16/+$1A plane B picture x/y, +$1E split line (copied to +$20 by the V-int; byte +$21 = H-int line), +$22 tilt warning countdown, +$24 the scene's V-int object; lift: +$26 floor split line $AF - shake (byte +$27 = H-int line), +$28 long = shake for the floor's VSRAM (Stage2_LiftVIntScroll); other users with other meanings: $3CEF8, $3DA2E-$3DE62, $4062A, $40980); do not give scene-specific global names to these addresses
g_VDPCmdBuf = $FFFF9910	; long: VDP command staged in RAM and sent as two word writes (high, then low word at +2), so the DMA-starting word comes from RAM: $74A8, TextMap_LoadFont; the V-int keeps a2 = this buffer (VInt_Handler), Stage1_VIntFireStep writes through (a2)
g_MapScrollY = $FFFFE00E	; long 16.16: map y offset of the auto-scrolling sections (map/terrain y = world y + it, camera fixed): the zeppelin lift sets $1910 ($32F14), $2410A adds g_LiftVelY each frame, Trigger_RunY keys on it (screen rows +0..$E0), $24F70 adds it for terrain tests, $33BD8 subtracts it; Debug_FreeCamStep shows cursor y + it (meaning outside these scenes inferred)
g_MapScrollX = $FFFFE00A	; long 16.16: map x offset of the auto-scrolling sections (the x twin of g_MapScrollY): $33274 adds $180 per frame, $24660 derives its parallax from it; Debug_FreeCamStep shows cursor x + it (inferred from these uses)
g_Rot3DOuter = $FFFFF3C4	; struct ($2A bytes, a 3D record): angles +0/+4/+8, translation +$C/+$10/+$14 (longs), matrix +$18-$28; the outer, perspective transform: Boss_Claw_DrawChain/DrawPoints project the claw's chain and fingers through it (Rot3D_ProjectPoint), the swing natives set its angles and +$14 depth (Boss_Claw_SetSwing); also $471C0
g_Rot3DInner = $FFFFF3EE	; struct ($2A bytes, a 3D record like g_Rot3DOuter): the claw fingers' own rotation (Rot3D_TransformPoint, no perspective): Boss_Claw_PlaceFingers adds 8 to angle +0 per call (spin) and $156 per finger; Boss_Claw_Spawn sets +0 = $2F8; Boss_Claw_SwingFingers swings angle +4
g_HScrollB = $FFFFDFB4	; word: plane B horizontal scroll, the low word of the long g_HScrollA writes to VRAM $BC00 ($AF10); the parade-truck section loop ($1FECC) subtracts $10 every frame (plane B streams left 16 px a frame behind the moving truck); $48C36 sets it
g_FxPhaseA = $FFFFF4B2	; byte: next frame phase for player shots: Op4_TakeHitPhaseA copies it to the shot's $18 and inverts it, so successive shots hit-test on alternate frames (vs g_FrameParity)
g_FxPhaseB = $FFFFF4B3	; byte: as g_FxPhaseA for enemy bullets (Op4_TakeHitPhaseB, tested by Op4_HitPlayersOnPhase)
g_LiftGoalVelY = $FFFFDFFE	; long 16.16: goal speed of the zeppelin loading-dock lift: Stage2_LiftStart sets -1.0 (up), Stage2_LiftStop 0; Stage2_LiftUpdate ramps g_LiftVelY toward it by $300 a frame and clears it at the top
g_Stage2ViewY = $FFFFE012	; long 16.16: view y of the stage 2 lift scene = g_MapScrollY + g_ShakeY (Stage2_UpdateScroll); the shaft's column V-scroll derives from it (Stage2_LiftColumnScroll)
g_Stage2BossPlaneOn = $FFFFF5F4	; byte: nonzero = Stage2_ArenaDrawTilt draws plane B's picture (the boss machine above the arena, inferred from png f033000) and its split line; set by the boss script native $25C2C, tested by $2661C
g_Stage2DebrisTimer = $FFFFF5F5	; byte: carry accumulator of the arena tilt's debris effects (+$24 per frame, an Scr4_024360 effect on each carry, Stage2_UpdateScroll); reset by Stage2_ArenaRequestTilt
g_Stage2TiltAllowed = $FFFFF5F6	; byte: nonzero lets Stage2_ArenaRequestTilt arm a floor tilt; set by the boss native $25C2C (fight start, TAS f32140), cleared by $25D2A (f33526)
g_Stage2BossPlaneAlt = $FFFFF5F7	; byte: nonzero = Stage2_ArenaDrawTilt shifts plane B's picture x by $200 (the other half of the plane: the wrecked machine, inferred from png f034800); set by $25DD8 (TAS f34683)
g_VM5SavedSP = $FFFFDFA6	; long: stack pointer saved by VM5_RunObject before it runs a wave script; the a5 yield/end ops (VM5_Yield, Op5_End, the Op5_WaitPopulation tail, the spawn-op tail $7080) restore it and rts
g_P1Flags = $FFFFF675	; byte: g_Player1 +$25 flags (bit 0 = in play, 1 = right-side HUD, 3 = Robin): bit 0 tested by Camera_TrackPlayersX/Y, SceneCb_CheckGameOver and the wave director (two-player caps and quotas)
g_P2Flags = $FFFFF6B5	; byte: g_Player2 +$25 flags, same bits as g_P1Flags; both bit 0 set = two players in play for VM5_RunObject and the a5 spawn-op quota
g_A6DmaSrc = $FFFFE03E	; long: source address of the a6 queued DMA job (Op6_QueueDMAEntry sets it, VM6_DMAStep advances it by the bytes queued each frame)
g_A6DmaDest = $FFFFE042	; word: VRAM destination of the a6 queued DMA job (Op6_QueueDMAEntry from the script, VM6_DMAStep advances it)
g_A6DmaTiles = $FFFFE044	; word: tiles left to send by the a6 queued DMA job (entry byte length / 32 from Op6_QueueDMAEntry; VM6_DMAStep subtracts what it queues and resumes the script at 0)
g_A6VIntJob = $FFFFE046	; word: V-int list object of the running a6 plane-fill / map-blit job (VM6_StartClearJob / VM6_StartBlitJob store it, VM6_EndClearJob / VM6_EndBlitJob free it through $8BF6)
g_A6TileWord = $FFFFE048	; word: tile word of the a6 plane jobs: the fill tile $5FC (VM6_SetupClearRect) or the value added to every map cell (VM6_SetupMapBlit, from the script)
g_A6MapStride = $FFFFE04A	; long: source row stride in bytes (map width * 2) of the a6 map blit (VM6_SetupMapBlit; VM6_BlitMapNow / VM6_VIntBlitRows add it to g_A6MapRowPtr)
g_A6RectCols = $FFFFE04E	; word: cells per row - 1 of the a6 plane fill / map blit (VM6_SetupClearRect / VM6_SetupMapBlit; dbra count of the row writers)
g_A6RectRows = $FFFFE050	; word: rows left - 1 of the a6 plane fill / map blit (VM6_SetupClearRect / VM6_SetupMapBlit; counted down by the V-int row writers, done when negative)
g_A6VdpCmd = $FFFFE052	; long: VDP VRAM-write command of the next row of the a6 plane fill / map blit (VM6_SetupClearRect / VM6_SetupMapBlit; advanced by g_PlaneRowBytes per row)
g_A6MapRowPtr = $FFFFE056	; long: source pointer of the next map row of the a6 map blit (VM6_SetupMapBlit; advanced by g_A6MapStride)
g_A6BankHandleBase = $FFFFE05A	; word: handle base passed as d1 to Spr_InitBankFrames by Op6_InitBankFrames (set to $8002 before every call; the stored-back d1 is unchanged and read nowhere else)
g_A6RedrawLayer = $FFFFE05C	; long: layer being redrawn one column per frame by the a6 redraw (VM6_StartRedraw sets it, VM6_RedrawStep reads it)
g_A6RedrawCols = $FFFFE060	; word: block columns left in the a6 layer redraw (layer +$3E = g_ScreenBlocksX, from VM6_StartRedraw; -1 per frame in VM6_RedrawStep)
g_A6RedrawSavedCol = $FFFFE062	; word: the layer's block column +$26, saved by VM6_StartRedraw and restored by VM6_RedrawStep after the last column
g_A6RedrawSavedCell = $FFFFE064	; word: the layer's plane cell column +$32, saved by VM6_StartRedraw and restored by VM6_RedrawStep after the last column
g_BeamRadius = $FFFFE5F6	; word: radius in px of the span disc that the stage 2 flight boss scene callback ($33A88) draws, with the beam cone from g_TriX0/Y0 (ship) to it at g_TriX1/Y1 (spot); 0 = none; copied each frame from the x of the boss's radius object (Boss_BeamShip_RadiusFromX / DeathRadiusFromX), cleared by Boss_BeamShip_ClearRadius; TAS 77185-79108
g_VIntListHead = $FFFFDE9C	; 8-byte head node of the V-int object list (+4 prev, +6 next; same node layout as the main list): VIntObj_RunList walks from it in every V-int (full and busy path), VIntObj_InitList links it to g_VIntListTail
g_VIntListTail = $FFFFDEA4	; tail sentinel of the V-int object list; handler = VIntObj_ListEnd (stored by VIntObj_InitList); VIntObj_New/VIntObj_NewIdle insert new objects before it (+4 = last object)
g_PlayerBoxTail = $FFFFDEB0	; tail sentinel node of g_PlayerBoxList (+0 next = 0, +2 = g_PlayerBoxLast); Hitbox_InitLists links it, Hitbox_LinkPlayerList stores #$DEB0 as each appended box's next
g_PlayerBoxLast = $FFFFDEB2	; word: prev link of g_PlayerBoxTail = the last box on g_PlayerBoxList (the head $DEAC when empty); Hitbox_LinkPlayerList appends after it and stores the new box
g_EnemyBoxTail = $FFFFDEB8	; tail sentinel node of g_EnemyBoxList (+0 next = 0, +2 = g_EnemyBoxLast); Hitbox_InitLists links it, Hitbox_LinkEnemyList stores #$DEB8 as each appended box's next
g_EnemyBoxLast = $FFFFDEBA	; word: prev link of g_EnemyBoxTail = the last box on g_EnemyBoxList (the head $DEB4 when empty); Hitbox_LinkEnemyList appends after it and stores the new box
g_Stage2TurretsDown = $FFFFF5F2	; word: bit mask of the stage 2 arena boss's destroyed turrets (bits 1/2/4/8 left to right, ORed in by Boss_Stage2_TurretDestroyed from each turret's var 0); Boss_Stage2_ReadTurretsDown copies it to the boss's var 0, $F = phase 2 (RAM: 3 at TAS f32700, $F at f33180); cleared with the scene RAM (inferred)
g_Stage3RoomSway = $FFFFF438	; word: wobble of the stage 3 spotted room (section $3300): a spring around 0 (Stage3_SpottedRoomScroll adds g_Stage3RoomSwayVel, which moves 4 toward it each frame); every plane A line's H-scroll and every column's V-scroll shear by it; starts $B00 (Stage3_SpottedRoomInitScroll)
g_Stage3RoomSwayVel = $FFFFF43A	; word: speed of g_Stage3RoomSway, +-4 per frame toward 0 (an undamped spring); starts $20 (Stage3_SpottedRoomInitScroll, Stage3_SpottedRoomScroll)
g_Stage3RoomSlideVel = $FFFFF43C	; long 16.16: speed of g_Stage3RoomSlide: -$1000 per frame down to -4.0 (Stage3_SpottedRoomScroll; 0 at the scene start, inferred from the RAM clear)
g_Stage3RoomSlide = $FFFFF440	; long 16.16: horizontal slide of the spotted room's pattern, kept within -40..+40 px (wraps by 80 px, one hole period); the base of the room's line H-scroll, ceiling and floor lines spread from it by /$92 per line (Stage3_SpottedRoomScroll)
g_ConeSlots = $FFFFF444	; 3 x $1E bytes ($F444-$F49D): stage 4 translucent cone slots: +0 byte in use, +1 mode (0 spin, 1 sweep once, 2 sweep, 3 close), +2 colour set offset into Tbl_047508 (0/8/$10), +4/+6 apex screen x/y, +8/+$A/+$C first angles, +$E/+$10/+$12 second angles, +$14 depth ($90), +$16/+$18 target width/length, +$1A/+$1C current width/length (Stage4_ConeAlloc/Update/SetInline/Free; drawn by Stage4_DrawCones)
g_ConeDrawSlot = $FFFFF49E	; word: index 0-2 of the cone slot Stage4_DrawCones draws this frame (round robin, one slot per frame)
g_ConeNextColour = $FFFFF4A0	; word: colour set 0-2 the next cone gets (Stage4_ConeAlloc cycles it; times 8 = offset into Tbl_047508)
g_ConeEdge = $FFFFF4A2	; 12 bytes: the cone's two far-edge points (x, y, z words), built from width/length and rotated in place by Stage4_DrawCones before projection
g_ConeClearCount = $FFFFF4F2	; byte: frames Stage4_DrawCones has cleared the spans since a cone was last drawn (stops at 2: both span buffers blank)
g_SceneSkipDelay = $FFFFE032	; word: frames before Start may skip a scene in Scene_LoopSkippable (counted down there, clamped at 0); $1E set by Scene_LoopSkippableDelayed
g_SndFadeLevel = $FFFFF6D4	; word: music fade/attenuation level 0-$FF; Snd_ResetFade clears it and sends 0 to Z80 RAM $0E67; the never-run stub $1E5030 -> $1E79D4 adds d0 (clamped to $FF) and sends a curve byte there (256-byte table $1E79F8: 0 -> 0, $80 -> $0F, $FF -> $7F); 0 = no fade is inferred (songs start right after it)
g_SndSfxSlots = $FFFFF6D6	; 6 x 6 bytes ($F6D6-$F6F9): sound-effect request slots; Snd_PlaySfx writes the slot at Tbl_1E50D6 entry byte 0 when the entry's byte 1 >= slot +0: +0 = entry byte 2 (priority kept), +1 = byte 3 (hold frames), +2.l = effect data pointer; Snd_SendSfxBody (every full V-int) copies a slot with a nonzero +2.l to Z80 RAM $17EA+6n and clears +2.l, else counts +1 down, then clears +0
g_OptMusicTest = $FFFFF60A	; word: options music test number (0-$F, shown +1), stepped by Menu_Update through Tbl_049994; Options_Step plays song value+1 with Sound_RestartMusic on A/B/C (scenario-options)
g_OptSoundTest = $FFFFF60C	; word: options sound test number (0-$39, shown +1), stepped by Menu_Update through Tbl_049994; Options_Step plays effect value+1 with Snd_PlaySfx on A/B/C (scenario-options)
g_ArtHandles = $FFFF8000	; word table at the base of work RAM: the sprite-art cache handles, cleared FF8000-FF8CFF by ArtCache_Init; Scene_InitCommon clears work RAM from here up to g_VIntCount
g_SceneBuf = $FFFF0000	; start of RAM: the scene decompression buffer (all 16 g_BufPtrs entries point here after Scene_ResetBufPtrs); Main_Start clears RAM from here
g_UnkGame_F634 = $FFFFF634	; word: g_UnkStart_F610 copied at a new game (Player_NewGameSetup); no reader found (meaning unknown)
g_FrameQuarter = $FFFFF4B7	; byte: += $40 by Frame_End every frame (a 4-frame cycle in bits 7-6); no reader under this address found (perhaps the low byte of a word read of g_FrameParity)
g_DebugHexFrame = $FFFFF436	; word: the sprite frame handle Debug_DrawHexBytes reuses for every hex digit it draws (Spr_ChangeFrame d2 in, d1 out; the free-camera readouts)
g_UnkClaw_F6D2 = $FFFFF6D2	; word: cleared every frame by Boss_Claw_DrawChain; no reader found (meaning unknown)
g_ObjPool = $FFFFAD54	; 140 x $5A bytes: the object pool's blocks, linked into the free list at g_ObjPoolHead by ObjPool_Init (Scene_InitCommon: a0 = $FFAD54, d0 = $8C)
g_UnkStart_F61A = $FFFFF61A	; word: new-game setting, 1 at boot (Main_Start); no reader found in the code
g_UnkStart_F61C = $FFFFF61C	; word: new-game setting, 1 at boot (Main_Start); no reader found in the code
g_CreditText = $FFFFED66	; 12 bytes: the 'CREDIT nn' panel text HudOp_ShowCredits builds (CREDIT, two spaces, g_Credits as 2 digits max 99, two spaces); pointed to by the player record's +$1A
g_DebugCursorBlockX = $FFFFF426	; word: the free-camera cursor's x rounded down to its 16-px block (g_DebugCursorX & $FFF0), drawn as frame $B4 and shown as a hex readout (Debug_FreeCamStep)
g_DebugCursorBlockY = $FFFFF428	; word: the free-camera cursor's y rounded down to its 16-px block (g_DebugCursorY & $FFF0); see g_DebugCursorBlockX (Debug_FreeCamStep)
g_DebugReadout = $FFFFF42A	; word: scratch value for one free-camera hex readout: the cursor's x, then y, offset from the view centre (cursor - camera - $A0 / $70), passed to Debug_DrawHexBytes in a6 (Debug_FreeCamStep)
g_DebugCursorMapX = $FFFFF42C	; long: the free-camera cursor's map x for its readout, g_MapScrollX + g_DebugCursorX (Debug_FreeCamStep)
g_DebugCursorMapY = $FFFFF430	; long: the free-camera cursor's map y for its readout, g_MapScrollY + g_DebugCursorY (Debug_FreeCamStep)
g_DebugCursorFrame = $FFFFF434	; word: the sprite frame handle Debug_FreeCamStep reuses for its two cursor sprites (frames $AA and $B4; Spr_ChangeFrame d2 in, d1 out)
