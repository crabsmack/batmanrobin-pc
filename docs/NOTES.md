# The Adventures of Batman and Robin: shared findings

Every worker appends here: this is how parallel sessions share understanding. Keep entries
short, with evidence (function names, addresses, frames). Sections grow as the game is learned.

## Triage (day 0, `dkit genesis triage`)

```
== Adventures of Batman & Robin, The (USA).md  (2048 KB)
   THE ADVENTURES OF BATMAN AND ROBIN | (C)SEGA 1995.APR | GM MK-1535 -00 | regions JU | checksum $C661 ok
   reset $000300  V-int $00007736  H-int $FFFFE560  SSP $00000000
   KIND: hand-written 68000 assembly
     - 2 LINK A6, 64 caller pops; calls mostly jsr abs/bsr (838 jsr abs, 2441 bsr)
   counts: LINK A6 2, UNLK 2, RTS 873, jsr abs 838, bsr 2441, jsr (An) 136, caller pops 64, movem push 93
   RAM vectors: {'IRQ4 / H-int': '$FFFFE560'}
   markers: Zyrinx Advanced Z80 Player; Zyrinx Advanced Z80 Player (Jesper Kyd)
   * 7 jsr/jmp to fixed RAM addresses ($FF9888, $FFC7CA, $FFE560): code copied into RAM runs on the recomp's interpreter (phase 2 K7/K8 count it); rule on it at the phase-2 kickoff (inferred from the opcodes: confirm with the corpus's ram_exec)
   * interrupt vectors point into RAM: code is copied/patched at run time — trace RAM execution and writes (dkit genesis run ... --trace-writes) and decide the recomp policy for it at kickoff
   * 20 high-entropy 32 KB blocks (compressed data): the decompressor is early work
   * no compiler to match: plan disassembly + recompilation; discovery is runtime-driven (static descent from vectors reaches little), so write corpus scenarios early
```

Kickoff corrections (2026-10-24): `$FF9888` and `$FFC7CA` are false positives (the "jsr" bytes lie
in compressed data / raw art and never ran); only `$FFE560` is RAM code. The game has no boot
checksum. Full kickoff measurements: docs/phases/P1-disasm.md "Kickoff measurements".

## Memory map and hardware use

ROM: $000000-$04FFFF code + VM scripts + tables; $050000-$05A022 raw tables/art; $05A022-$0FFD3E
103 LZ blocks (one raw art gap $0B29C5-$0B3CEA); $100000-$1CFFFF uncompressed art (DMA source;
sprite headers read by $7F70, $68AC); $1D0000-$1E27F0 tables (read by $7BC8, $7F0C, $6898);
$1E27F0-$1E500E $00/$FF fill; $1E500E- sound code, Z80 blob $1E6104-$1E7928, music banks
$1E8000/$1F0000/$1F8000 to the ROM end.

RAM code: H-int at $FFE560 (the vector). Variant A, the per-line VSRAM raster handler: `$00B1B6` copies
$4C bytes from $00B20C (only $B20C-$B231 is the handler; $B232-$B257 comes along and never runs);
it rewrites its own operand ($FFE562) and branch displacement ($FFE56D: 02→0E disables H-int);
ROM re-patches it at $00B1D8/$00B1F0 (line tables $FF9040/$FF9240, swapped at $B1E0) and
$00B1F4. Variant B: `move.w #$4EF9,$FFE560` + `move.l #target,$FFE562` jmp stubs from
$01C450 (→$01C468), $024506 (→$024526), $0245B6 (→$0245E0).

## Subsystems

**Threaded-code VMs** (the game's main structure). Four interpreters; a handler ends in NEXT
`movea.w (aR)+,a0; jmp (a0)` (handler addresses are words: all handlers lie below $8000).

| VM reg | handlers | entry / resume |
|---|---|---|
| a2 | 438, $07FC-$5378 | `jsr $7FE.w` (pops return address into a2; 73 sites); $0007D2 resumes an object's script from $2A(a4), saves sp in $DEC2 |
| a4 | 153, $53C4-$69A8 | `jsr $53C6.w` (2 sites); $0053B4 resumes from $8(a6) |
| a5 | 49, $6A72-$6FD2 | `VM5_RunObject` $6A14 (wave directors) |
| a6 | 22, $70B8-$7682 | `VM6_Run` $70A4 (scene setups), `VM6_RunHook` $7086 from Frame_End |

- $037134: inline-argument object spawn (`jsr` + `dc.w id; dc.l params; dc.l script`, 10 bytes),
  stores the script pointer in $2A(a4) and returns to the caller's caller.
- These three are `[disasm] noreturn` in dkit.toml: the bytes after the call are script/args.
- Native fragments: $07FC (`jmp (a2)`, a2) and $53C4 (`jmp (a4)`, a4) drop into the 68000 code
  that follows; native code re-enters script with `jsr $7FE.w`. Native-call ops (`jsr (a0)`):
  $2D30 (relative), $2D40 and $5438 (`dc.l`), $244A (`dc.w`, `dc.l`). Example: $03D934 is a native
  callback entered by `jsr (a0)` at $008DAC.
- Yield/end: e.g. $0820 `adda.w (a2),a2; move.l a2,$2A(a4); movea.l $DEC2,a7; rts`; a4's $53E8, $53EE.
- Branches: relative `adda.w (aR),aR` (96 a2 / 23 a4 handlers); absolute goto $2D1A, $5422;
  gosub $2D10, $53F6 (`pea 2(aR); adda.w`); return $2D2A (`movea.l (a7)+,a2`); jump table $3714.
- Operand sizes (a2): 148 handlers take 0 bytes, 117 take 2, 49 take 4, 10 take 6, 8 take 8; ~100 vary by path.
- Scripts: ~79 KB read in the TAS (a2 54,468, a4 16,408, a5 5,992, a6 1,756), ~3,000 pieces
  $002D28-$1E252B. Phase 1 labels them as data spans (ruling 2026-10-24); op-level decoding is phase 4.

**Decompressor** `Lz_Decompress` $00A23E (output end -> g_LzOutEnd $E08E): LZ77, separate literal stream (header long = offset to literals), 16-bit
MSB-first control bits (0 literal, 1 match; lengths 2 / 3-5 / 6-20 / 21-275 in 1/2/4/8-bit tiers,
8-bit 0 ends; distance class 2 bits then 5/7/9/10 bits, up to 1696 back). Runs synchronously
(VM op $7118) or as a pre-emptible background coroutine ($7120 → `jmp $A1F0`; context at $E066;
$A20C resumes with a fake `rte`; the V-int at $77E0 captures the interrupted PC/SR).

**Sound driver (68000 side)** (Zyrinx Advanced Z80 Player, $1E500E-$1E7D8B). API = `jsr (a1)` stubs
at $1E500E-$1E50D6 (a1 preserved) in front of driver bodies at $1E799A-$1E7D8B; every Z80 RAM
write is a byte store repeated until it reads back. `Snd_Init` (boot, $9E02): Z80 reset off,
`Snd_LoadDriver` (copy Snd_1E6104, $1825 bytes, to $A00000; d1 = $E4 into Z80 $003B; reset
pulse), `Snd_StopChannels`, `Snd_ResetFade`. Music: `Sound_PlayMusic` -> `Sound_RestartMusic` ->
`Snd_StopChannels`, `Snd_ResetFade`, `Snd_PlaySong` (song table $1E6028: per song word
song-in-module + word module offset 0/8/$10; songs 1-$13) -> `Snd_StartSong(Body)` (module
entry Tbl_1E7EE4 = long 3-byte song-pointer list + long module header, banks $1E8000/$1F0000/
$1F8000; Z80 held in reset while `Snd_InitSongChannel` fills the six channel records
$A01480+$67n: bank +$2D, window address +$2E/+$2F, header bytes +$2B/+$29, 3 bytes per channel
from $1E7914 into +$2A/+$20/+$21, on flag +$1F; `Snd_StartSongFinish` writes two 3-byte bank
pointers from the module header to $0E61-$0E66 and restarts the Z80). Effects: `Snd_PlaySfx` (d0
1-based, 8-byte entries Tbl_1E50D6: slot offset, priority, kept priority, hold frames, long data
into Snd_1E53A6) fills a g_SndSfxSlots slot; the V-int's `Snd_SendSfx` copies pending slots to
Z80 $17EA+6n. Fade: g_SndFadeLevel -> curve $1E79F8 -> Z80 $0E67 (`Snd_WriteFadeByte`; only the
reset to 0 ever runs). Never run, left raw: stubs $1E5022 (Snd_WriteFadeByte), $1E5030 (fade
step $1E79D4: level += d0, clamp $FF), $1E5058 (-> $1E7B6C: Snd_StartSongBody keeping the caller's a3), $1E5090 (-> $1E7D8C: d0 into Z80 $00ED); bodies
$1E79D4-$1E7AF7, $1E7D8C-$1E7EE3; the song table's tail $1E6078-$1E6103 is not song entries.

**Background task and preemption** (phase 2, 2026-10-26). `$00A1F0` starts a background task: it
sets $F4BC, a4 = $A23E (the decompressor), and saves a context at $E066. `$00A20C` resumes it: it
sets $F4BD, restores the context and does a fake RTE into it. The V-int (tail at $0077E4), when
$F4BD is set, pops the interrupted SR/PC into the context and returns to the task's caller. The
task's code can therefore be parked and resumed at any instruction. dkit.toml `[recomp]
resumable` labels $00A23E-$00A3AE, and also $00DAB4-$00E81A: in fuzz-77400-s1 the recomp
resumed into $00DC2E/$00DC64/$00DCAC/$00DDA8. That PC wasn't the decompressor's. Inferred, not
verified: the V-int re-enables interrupts mid-handler (`andi.w #$F8FF,sr`), so a nested V-int
during a long frame can capture V-int-time code as the "task" PC. A real game race, reproduced
exactly by the recomp (frames equal).

**The decompressor is a bulk reader, not a checksum.** Since the `level-skip` scenario loads every
stage, `dkit disasm coverage` warns that "$00A240 reads 547,605 ROM bytes (26%): a boot checksum?".
It is the LZ decompressor's literal/bitstream reads of the compressed blocks. Don't add it to
`[coverage] ignore_readers`: that would drop real data reads from S4. The blocks get labelled.

**V-int and frame handshake** (`VInt_Handler` $7736). The main loop ($9BF0) clears g_FrameBusy
($F6FB) when a frame is ready and waits for g_VIntDone ($F6FA); while g_FrameBusy is set the
V-int takes `VInt_BusyPath` ($77FC: V-int object list $8ADA, $AEBC, pads $A3D4, no DMA/sound/fade).
g_VIntOff ($F6FC) makes it a bare RTE (scene init $9AAC).

**DMA queue** $991C: 40 entries x 14 bytes (VDP regs $93-$97 + command long). `DMA_ResetQueue`
($7836, frame start) sets 40 free slots and the cost budget g_DMABudgetLeft = g_DMABudget (per
scene). Writers call `DMA_Reserve` ($799C: d0 slots, d1 cost; a0 = write ptr, d0 = 0/-1), write
entries, store a0 to g_DMAQueuePtr. `DMA_FlushQueue` ($784A) sends them in the V-int through a
computed jump into an unrolled copy (`DMA_FlushQueue_38`/`_29` are entries into it).

**Sprite display list** (per frame; `Spr_ResetList` $7A08 at frame start, 80 entries). Add with
`Spr_AddSorted` ($7A4A, descending d7 = in front first), `Spr_AddFront` ($7B50, before all sorted)
or `Spr_AddBack` ($7B1C, after all); inputs a0 = 10-byte frame-table entry (word art handle, long
piece list, long art), d4 x, d5 y, d6 attr (bits 11/12 flip). `Spr_BuildSAT` ($7B72, end of
frame) writes the SAT buffer $FF9B56, `Spr_QueueSATDMA` ($79C0) queues it to VRAM $0000.
`Spr_DrawDirect` ($7D5E) bypasses the list (HUD digits).

**Sprite frames and the VRAM art cache.** A frame number is a byte offset into a table of 10-byte
entries (word art-handle address, long piece list, long art): >= 0 → ROM `Tbl_1DDB60`, < 0 → RAM
table below $A764 (filled by `Spr_InitBankFrames` $7EB8 from loaded banks). `Spr_ChangeFrame`
($7EF6, 37 sites) swaps a sprite's frame; when the art handle differs, `Spr_SwapFrameArt` ($7F5E)
acquires the new art (`ArtCache_Alloc` $85F8 + `Spr_QueueArtDMA` $8042) and releases the old;
if VRAM or DMA budget is short it keeps the old frame. VRAM is split into 16-tile blocks from tile
$5F (`ArtCache_Init` $8514, count from the scene script). Handles (RAM $8000-$8CFF) point to
$1E-byte descriptors ($A774, 50): refcount byte (negative = users, 0 = cached/evictable), block
count, release-ring flag, back pointer, block tiles. Released art stays in VRAM on the release
ring ($8F00) until `ArtCache_Alloc` evicts it. Free blocks $8D00, free descriptors $8E00 (256-byte
rings, pointers $A764-$A76E).

**Objects.** Pool of 140 x $5A-byte blocks at $AD54 (`ObjPool_Init` $86E6 at scene init;
`ObjPool_Alloc` $86FE, `ObjPool_AllocClear` $8718, `ObjPool_Free` $8764, `ObjPool_HasFree` $8772).
An entity may use several blocks (e.g. $101CE takes 3 for a3/a4/a5). Main object list: head
$DE8C, tail sentinel $DE94 (`Obj_InitList` $88C2 at scene init); entry +0 handler long, +4 prev,
+6 next. `Obj_RunList` ($87B6) calls every handler with a6 = object; the sentinel's handler
`Obj_ListEnd` ($88BE) ends the walk. Creating (all append before the tail, return a6 and d0 =
0/-1): `Obj_New` (1 block, $8A08), `Obj_NewClear` (+ zero +8..+$17), `Obj_NewMulti` ($8912: d7
blocks, the other d7-1 block addresses as handles from +8, the rest of the 8 handle words +8..+$17
zeroed, by two computed jumps into unrolled runs: `_HandleRun` entry $896C-4n, `_ClearRun` entry
$896A+2n), `Obj_NewMultiUnlinked` ($8984, +4/+6 = 0), `Obj_LinkAtEnd` (link a built object later).
Freeing: `Obj_UnlinkFree` (1 block), `Obj_FreeWithBlocks` (a6 + every nonzero handle +8..+$17;
no unlink) and its unlinking entry `Obj_UnlinkFreeWithBlocks` (Obj_Destroy picks by +4 != 0).
**V-int object list** (g_VIntListHead $DE9C / g_VIntListTail $DEA4, same layout): `VIntObj_RunList`
($8ADA) walks it in every V-int, full or busy path; `VIntObj_New` (handler a0), `VIntObj_NewIdle`
(handler = an rts, the caller stores its own) and `VIntObj_UnlinkFree` raise g_VIntOff around the
link edit. Box lists: `Hitbox_InitLists` ($8C1A), `Hitbox_LinkPlayerList`/`LinkEnemyList` append
after g_PlayerBoxLast/g_EnemyBoxLast (the tails' +2 prev). Raw, never run: $88F6 unlink a0 from
the main list, $899A NewMulti inserting after the current a6, $8B3E/$8B62 link/unlink a0 on the
V-int list.

**Boot.** `Boot_Reset` ($300, standard Sega init; a soft reset skips it) → `Boot_CheckRegion`
($3FA: console region vs header "JU"; mismatch = lockout text via `Boot_DrawLockoutText`, hang)
→ `Boot_InitIOPorts` ($790: pad ports TH out) → `jmp $9E02` (RAM clear, main init).

**a2 script objects** (from the ops $07FC-$16E6). `VM2_RunObject` ($7D2) is the object handler:
saves sp in g_VM2SavedSP, a3/a4/a5 = handles $8/$A/$C(a6), a2 = $2A(a4); its tail **$7E0 is
NEXT** (every numeric `$7E0` branch target = "continue"), $7E4 `Op2_SkipWord` = branch not
taken, $2D20/$2D22 = branch taken (skip a word / `adda.w (a2),a2`), $2D2A = return (pop a2).
`Op2_Yield` $822 (d0 = 0) and `Op2_Kill` $95C (`Obj_Destroy` $980, d0 = -1) end a frame.
Fields: a6 +$18 population weight (sum in g_ObjPopulation), +$19/$1A spawn action (Tbl_012EF0
index, arg), +$32/$36/$3A script vectors, +$3E/$42 x/y 16.16 (+$46/$4A saved copy), +$58 owner
id (1/2 players, $A/$B enemies). a4: +$1A/$1C/$1E/$20 timers 0-3 (counted down by the tick
$30(a4) = speed $2E(a4) >> 4, `Op2_SetSpeed` $AF2), +$2A resume pc, +$36 points, +$38/$3C/$40/$44
**coroutine slots 0-3** (started by $2334-$240C, stepped by $235A/$23A2/$23EA/$2432: push a2,
a2 = slot; a coroutine yields with `move.l a2,slot / movea.l (a7)+,a2`, e.g.
`Op2_SetTimerNYieldM`, `Op2_WaitTimerN`). a3 motion block: +0 bit 3 vertical-dominant / bit 4
negative direction (`Mot_SetTarget` $13CA), +$A target heading, +$C.l current heading (0-$3FF,
1024 per turn, sine table Tbl_059B22 with cos at +$100), +$10 turn rate, +$14/$16 target point,
+$18/$1C velocity, +$20/$28 accel, +$24/$2C target speed, +$30/$34 cruise speed. a5 sprite
block: frames from +0 (count +$14), +$18 bit 0 anim flag, +$2E anim script, +$38 attr word
(d6, byte bit 3 = H flip), +$3A sort depth (d7).

**a2 motion, terrain and variable ops** ($1708-$2334). The steering tail $4B8A moves each
velocity ($18/$1C) toward its target speed ($24/$2C) by its acceleration ($20/$28) every run,
copies it to $38/$3C(a3) and adds the decaying impulses $2/$6(a3). `Op2_Cruise{Right,Left,Up,
Down,Forward}` set a target speed to +-|cruise| ($30/$34); the seek ops (`Op2_SeekScreenX*`,
`...Y*`, `...World*`, `...Range`, `...OrBranch`) compare the screen (minus g_CameraX/Y) or world
position with the target point $14/$16(a3) or a [lo, hi] word pair and cruise toward it, clearing
the target speed (and branching, for OrBranch) on arrival. Jumps: `Mot_JumpGravity` ($1D7E: words
height h, time T) returns g = 2h*65536*t^2/(T(T+t)), t = $2E(a4)/16; `Mot_JumpLaunchSpeed` = -g*T/t;
`Op2_Jump`/`Op2_JumpAnim`/`Op2_FallAnim`/`Op2_ArcX` use them with the cruise speed as the fall
limit. a5 +$32.l/+$36.w = velocity-to-frame bias/divisor (frame ops $3EE2/$3F1C pick a case from
(v + $32)/$36). Terrain boxes for the move op $4CDA: box A = bit 1 of (a3), offset $40/$42, wall/floor
masks $44/$46 (contact zeroes that velocity); box B = bit 2, $48/$4A, $4C/$4E, collided first;
results $50 (d3), $52 wall, $54 floor (`Op2_Branch{WallMask,FloorMask,OnFloor,OffFloor,...}`);
bit 0 of (a3) = g_TerrainFnAlt (`Op2_UseAltTerrain`). a4 +$22/$24/$26/$28 = **script variables
0-3** (words; set/or/and/inc, loop, bit-test branches, `Op2_SwitchVar0Bits`/`Var2Bits` bit
switches; players keep pad bits in var 0, flags such as $400 in var 1).

**a2 coroutine slots and child objects** ($2334-$2C06). Each slot N (a4 +$38/$3C/$40/$44) has
`Op2_StopCoroutineN` (= Scr2_002D28, just the return op), `Op2_StartCoroutineN{Here,Rel}` (slot 0
also `Abs`) and `Op2_StepCoroutineN` (push a2, run the slot until the return op $2D2A). A slot can
also hold native code: `Op2_CallSlot2Native` ($23E0) jsr's it (stage 1 loop $1E7B4, slot 2 =
$1EA24); never-run ops set a slot to the native no-op rts $82C or call slots 0/1. **Child
objects**: a word operand names an a6 field that holds a child's a6 (0 = none): `Op2_SpawnChild`
($244A: word field, long spawner; only when empty), `...OrWait` (yields and retries), `Op2_KillChild`,
`Op2_BranchHasChild`/`NoChild`, `Op2_RunChild` (runs the child's handler inline), `Obj_SyncChild`
($2550: flag bits copy x/y, drawn position, offsets, sprite attr/depth to the child), and
`Op2_{Start,Stop}ChildCoroutineN` ($2AA6-$2BF6, via `Child_GetA4`). Loop counters 0-2 = a4
+$14/$16/$18 (`Op2_SetCounterN`, `Op2_LoopCounterN`: -1 per run, unlike the tick timers). Flow
($2CB2-$2D40): `Op2_CallNative` $2CBC (dc.l) / `...Rel` / `...RelSaved`, `Op2_CallNative{,Rel}Switch`
(branch table by the returned d0), `Op2_Gosub` $2D10 (interior label), `Op2_GosubArgs`/`AbsArgs`
(4 words into g_VM2Arg3..0 first), `Op2_Goto` $2D22, `Op2_GotoAbs`, `Op2_Return` $2D2A.

**Player targeting** ($2D4E-$356A). a4 +$10 = the targeted player record (g_Player1/2 word, 0 =
none). Record fields kept by the player handler $102D6 each frame: +$2E/+$32 x/y (16.16 copies),
+$3A targetable (var 1 bit 6), +$36 = attackers last frame (= +$38, then +$38 cleared); enemies
add themselves to +$38 each frame (`Op2_KeepPlayerTarget` etc.). `Op2_BalancePlayerTarget` /
`Op2_SwapPlayerTarget` / the picker $2DA2 choose the targetable player with fewer attackers (ties
alternate, g_TargetTieFlip). Then: `Op2_BranchPlayer{Right,Left,Below,Above}`, `Op2_FacePlayer*`,
`Op2_{Face}CruiseToPlayer{X,Y}`, `Op2_HoldRangeOfPlayerX*` (stand [near, far] px beside it in screen
space), `Op2_HoldPlayerWindow` (window packed as half-pixel bytes in a3 +$14/$16 by
`Mot_StorePlayerWindow`), `Op2_BranchPlayerInBox` (its box id = record +$3E). a6 +$1C = a random
distance jitter (`Op2_SetRandJitter`).

**Object boxes, energy and hits** ($3714-$3AA6). A script object owns two box sets in its a4
block: **body boxes** (+$2 count, +$4.. handles; `Obj_SetBodyBoxes` via `Op2_SetBodyBox{,es}`,
`Op2_ClearBodyBoxes`; placed and linked each frame by the op $4EDA) and **attack boxes** (+$A
count, +$C.. handles; `Obj_SetAttackBoxes` via `Op2_SetAttackBox{,es}`, tested by the op $4DA4).
Inline spec: words kind (jump-table offset: 0/4 player list, 8 enemy list), left, top, right,
bottom (relative, mirrored by the flip bits), word n (0, or n words = long hit callback + args,
e.g. `000105C6 0020 0300`). Box +4 owner serial, +$10 id (owner $58; `Op2_SetBodyBoxId`), +$2E
owner a6, +$22.l knockback push (written by the hit callbacks). `Op2_BranchStruck`/`NotStruck`/
`BoxStruck` test-and-clear the struck bit -> g_StruckBox; `Op2_TakeDamage`/`TakeKnockback`/`TakeHit`
apply it. Energy a6 +$52 (`Op2_SetEnergy` $39BA / `...1P2P` $39AC, scaled by g_EnemyEnergyScale or
g_BossEnergyScale), +$54 = 1% unit (`Op2_BranchEnergyAtPct`); `Op2_SwitchAttacker` branches on
the striker's id. `Op2_BranchTouchBox` ($3714) = overlap test against a whole list -> g_FoundBox.
`Op2_SetVector0/1` set the a6 +$32/+$36 script vectors.

**Script vectors and extra sprite blocks** ($3ABE-$3D58). Vectors 0-2 = a6 +$32/$36/$3A.
`Op2_SetVectorNHereGoto` (801 uses for N=0) store the op after their offset word in the vector
and jump to the offset: a jump that leaves a return point; `...Arg`/`Args2`/`Args4` variants first
load g_VM2Arg0 (..Arg3). `Op2_GotoVectorN` / `Op2_GosubVectorN` (push a2 for Op2_Return) /
`Op2_GotoVector2Plus` / `GosubVector2Plus` (vector + word) come back. An object can own extra
sprite blocks in a6 fields (like the a3 sub-blocks): `Op2_AllocSprBlock` (pool block, retries next
frame when the pool is empty), `Op2_LoadA5Handle` / `Op2_ResetA5` select one / the main $C(a6),
`Op2_ResetSprBlock`, `Op2_SetBlockAnim`, `Op2_Release{,Block}Frames`. a5 +$1A = aim direction x4
(`Op2_AimDirAtPlayer` via Mot_DirToPlayer, `Op2_SwitchAimDir`). `Op2_PushPos`/`PopPos` keep x/y on
the 68000 stack within one run; `Op2_BranchWeaponLvlMaxGE` (+ never-run stat stubs $3CDC-$3D54).

**Sprite animation and draw lists** ($3E58-$4AD1). Each sprite block (a5) runs two nested a2
scripts. **Animation script** at a5 +$2E: `Op2_RunAnim` (every object loop) counts the timer +$1E
down by the speed $2E(a4) and, when it runs out, `jsr`s into the animation script; its ops chain by
NEXT until one returns with rts: `Op2_AnimFrame` (inline draw list -> +$2A, timer += display time
+$1C, $100 = 1 frame), `Op2_AnimYield`, `Op2_AnimGotoYield`, `Op2_AnimStop` (sets the +$18 bit 0
done flag), `Op2_AnimHoldByVelY` (per-frame case by y speed). Timing: `Op2_SetAnimDelay{,Rand}`,
`Op2_AddAnimDelay`; loops: +$20 (`Op2_SetAnimLoop{,Rand}`, `Op2_AnimLoop`). Ordinary a2 ops (boxes,
sound, goto) also appear in animation scripts. **Draw list** at a5 +$2A: `Op2_Draw` runs it with a1
= the frame slots (a5 +0, count +$14); each `Op2_DrawSpr*` op does Spr_ChangeFrame on the next slot
and adds one sprite: suffix Attr = attr high byte (and, xor), Depth = +depth, Off = mirrored byte
offset, Front/Back = Spr_AddFront/AddBack, AtDrawn = at a6 +$4E/$50; `Op2_DrawEnd` (word $44F2,
1837 uses) releases the slots no longer used and returns to the script. a5 +$16 byte blink timer
(odd frames hidden; the player's spawn invulnerability), +$17 shake timer (-2..1 px jitter), +$22/
$24/$26 sprite variables (`Op2_SetSprVarN`; player natives step them). Also here: `Op2_Switch
{VelX,VelY}Band`, `Op2_SwitchHeading{,Clamp}`, `Op2_PlaySound` (Snd_PlaySfx), `Op2_PlayMusic`,
`Op2_WaitPopBelow*` (g_ObjPopulation), `Op2_StartA6Script` + `Op2_WaitA6Script*`: one global a6-VM
**loader script** (g_A6ScriptPc, decompress/DMA jobs, e.g. $1FB1E `7120 ...`) resumed once a frame
from Frame_End by $7086 through g_A6ResumeHook; g_A6ScriptBusy while it runs.

**Frame move, attacks and body boxes** ($4B74-$5077). The per-frame move passes **d1/d2** (16.16)
from op to op: `Op2_StepVelocity` ($4B74, every object: velocity toward target speed + impulses;
stun count $1(a3) freezes it) -> optional `Op2_ClampMoveToArea{,X,Y}` (g_MoveArea*),
`Op2_SubLiftVelY`, `Op2_AddOsc` -> `Op2_MoveTerrain` ($4CDA, Coll_MoveScene per terrain box) or
`Op2_ApplyMove`. Attacks: `Op2_TestAttackBoxes` ($4DA4; skipped while the pause count $1(a4)
runs) -> `Hitbox_TestAttackBoxAt`: the box kind word picks 0 = first player-list box, 4 = all of
them, 8 = first enemy-list box; `Hitbox_Strike` stores the attacker id at +$1A of the struck box,
calls the attack box's callback (+$30) then the struck box's. Body boxes: `Op2_PlaceBodyBoxes`
($4EDA, every object) writes each box's absolute rect and owner energy and links it by kind
(`Hitbox_LinkByKind`: 0/4 player list, 8/$C enemy list); a nonzero byte (a4) unlinks them
(never seen). **Oscillator** block in an a6 field (`Op2_AllocOsc`, `Op2_SetOscX/Y`, two axes of
5 longs: position, limit, velocity, max speed, accel): `Op2_StepOsc` ($50E2) / `Op2_AddOsc` ($50E6)
add a velocity that swings between -limit and +limit (hovering enemies); `Op2_StopOsc{,Y}`.

**a2 effect-spawn ops** ($515E-$5377). Inline Fx_RunScript: run an a4 spawn script (relative word,
or dc.l for `...Abs`) on g_SpawnCtx with velocity = the object's ($18/$1C(a3); `...Impulse` the
impulse $2/$6, `...Impulse34` 3/4 of it), attr/depth from a5. `Op2_FxSpawnHere{,Abs}` /
`Op2_FxSpawnOff{,Abs}` set the position themselves; `Op2_FxSpawnRegs*` use registers that earlier
ops prepared: `Op2_FxPos{Here,Anchor,Drawn,ScreenCentre}` (d4/d5, clear d2 radius / d3 heading),
`Op2_FxPosAdd{B,W,Flip}`, `Op2_FxHeading{,Here,ByFacing,Snap,SnapClamp}` (d3 high word), e.g.
`5294 F6E3 5306 0300 5198 0104` = offset, heading $300, spawn.

**Script objects and spawning** ($8DB6-$8FFC). A script object is 4 pool blocks: a6 (+8/+$A/+$C
= blocks a3/a4/a5), handler VM2_RunObject ($7D2) running the a2 script at $2A(a4).
`Obj_Spawn` ($8F0E: handler a0, d7 blocks, x/y d4/d5) / `Obj_SpawnUnlinked` ($8F24, not put on
the run list) + `Obj_InitSpawned` ($8F4E: coroutine slots a4+$38..$44 = Scr2_002D28, serial
$56 = g_ObjSerial++, a5+$38 = g_SceneSprAttr). Wrappers set the script a2 and the owner class
$58: `Obj_SpawnEnemy*` $A (199 spawners `lea Scr2_x,a2 / jmp Obj_SpawnEnemy`), `Obj_SpawnBoss*`
$B; `...OrQuit` variants pop the caller's return address on failure. `Spawn_At*` ($8D12-$8DB4)
are native callbacks (op $2CBC `dc.l`) that call such a spawner at a position (screen centre,
camera offset, own x/y, flipped offset, anchor $46/$4A, drawn position $4E/$50).

**Collision boxes** ($8C1A-$8D10). Boxes are pool blocks on two doubly linked lists (+0 next,
+2 prev, sentinel next = 0): `g_PlayerBoxList` $DEAC (the players' boxes) and `g_EnemyBoxList`
$DEB4. Box +$10 id, +$12/$14/$16/$18 rect l/t/r/b, +$1A attacker id (stored by Hitbox_Strike, then overwritten with the hit type 6-9 by the hit callbacks), +$1E owner energy, +$20
damage taken, +$2E owner, +$3C bit 0 struck. `Hitbox_FindPlayerBox`/`FindEnemyBox` + 
`Hitbox_FindNextBox` scan for overlap with d4-d6 x d5-d7 (Z clear, a2 = box);
`Hitbox_WipeEnemies` ($8FFE) damages every enemy box by $200 and sets g_ScreenClearTimer.

**a4 effect spawns** ($9010-$9248). `Fx_RunScript` ($91F2) runs an a4 spawn script on
g_SpawnCtx: registers d2/d3/d4/d5 become the new objects' radius $26, heading $2A, x $2E, y $32
(op $613A); ctx +$3E/$42 are a **velocity** added to theirs (ops $55D8/$560E add ctx $36-$42 to
the object's speeds $36-$42, integrated by $6286), not a position. `Fx_SpawnAt*` / `Aimed*` /
`Knockback` are a2 native callbacks (op $2CBC: `dc.w` args, `dc.l` a4 script). a4 object
display = (x, y) + radius * (cos, sin)(heading) (`$624C`, Tbl_059B22). `Arc_*` ($9266-$930A,
a4 natives via op $5438) plan and step a gravity arc to a point.

**a4 effect scripts** (ops $53B4-$5E66 named `Op4_*`, VM entries `VM4_*`). An a4 object is one
$5A pool block on the main object list, handler `VM4_RunObject` ($53B4: a5 = a6, a4 = $8(a6),
NEXT = `VM4_Next` $53BA). Block: +$8 resume pc, +$C anim script, +$10 owner id (inherited;
shots: player pad + 2), +$12 damage (`Shot_DamageTarget`), +$14 collision box, +$16 timer word,
+$19 counter byte, +$1A/$1B ticker accumulator/rate bytes (fires on the byte carry), +$1C/$1D
anim tick, +$1E wanted / +$20 shown sprite frame, +$22 sprite attr, +$24 depth (0 front, 1 back,
else sorted), then 12 longs (16.16): +$26 radius, +$2A heading, +$2E x, +$32 y, +$36-$42 their
velocities, +$46-$52 their accelerations; +$56/$58 drawn x/y words. Drawn point = (x, y) + radius
* (cos, sin)(heading). Registers: d2-d5 = radius/heading/x/y for the next spawn (16.16; `$63A2`
loads the own ones), after the motion ops d4/d5 = drawn x/y words; **a5 = the last spawned
child** (a6 until a spawn), so every `Op4_Child*` op edits the child just made. Ends: an op that
`rts`es: `Op4_Return` ($53F4, plain rts: ends a spawn script run by `Fx_RunScript`, a frame, or
an `Op4_Gosub`), `Op4_YieldGoto`/`Op4_YieldGotoAbs` (resume elsewhere), `Op4_WaitTicker`,
`Op4_Kill` ($5406: frees box, frame art and block). Spawn ops (`Op4_Spawn` $5606, `...AddVel`,
`...RunChild`, `Op4_ReplaceSelf`) take 3 words: failure offset (0 = end the run when the pool
is full), child-script offset, parameter-table offset (0 = start at d2-d5). Families: `Op4_Add*`
(word px, or word 8.8 for Vel/Accel), `Op4_AddMotion*` (3 longs: value, velocity, accel),
random: `Jitter*` +-w/2, `Rand*` [0, w), `Range*` base + [0, w), `Scatter*` +-(base + [0, w/2)).
`Op2_FxMarkPlayerTarget` ($5378, an a2 op) stores the targeted player's x/y in g_ArcTargetX/Y.
The parameter table (`VM4_SpawnChild` $613A) is 4 triples of (value, velocity, accel) longs for
x, y, radius, heading (+0/+$C/+$18/+$24); values add to d4/d5/d2/d3. `Op4_Fork*` ($607C-$60C2,
core `VM4_Fork` $60C4) are the cheap spawns: one script word, start exactly at d2-d5;
`Op4_ForkRestAndEnd` gives the child the rest of the script and ends the parent's run (an emitter
that resumes there spawns one per frame). Per-frame loop of most effects: a motion op (`Op4_Move`
all axes, `Op4_MoveXY`, `Op4_MovePolar`, `Op4_LoadXY` still) sets d4/d5, then tests (`Op4_TerrainMove*`
against g_TerrainFn from the last position $56/$58, `Op4_BranchTouchPlayer`, `Op4_HitPlayers`
callback per box, `Op4_BodyBox` shootable box with energy), then a draw op that ends the frame:
`Op4_Draw` ($67EE; the `Op4_DrawKill*` variants first delete off-screen / screen-clear objects).
**Animation**: `Op4_SetAnim` ($6418: ticker word $1C, anim-script long $C, resume here); `Op4_Draw`
adds rate $1D to $1C and on the carry runs the anim script, whose `Op4_AnimFrame` ($682C) /
`...Attr` ($6820) ops set the frame $1E, save the anim pc and draw (`Op4_DrawFrame` $6834 swaps
the frame art through the VRAM art cache inline); an anim script often ends in `Op4_Kill`.
**Player shots** (Scr4_05034A): `Op4_TakeHitPhaseA` (g_FxPhaseA: alternate shots test on
alternate frames vs g_FrameParity), `Op4_SetVelFromHeading`, `Op4_SetShotFrames` (a cyclic
{next, sprite} ring, drawn straight onto the front list without the art cache), `Op4_ShotStep`
(move, enemy-box test on its phase, off-camera kill on the other frame).
**In C** (P4, $5078-$5D73, w05): the object is `Vm4Obj` (types/VM4.h; the four motion values as
integer + fraction words, `vm4_fix` / `vm4_add_fix` / `vm4_add_long` for the long arithmetic).
Code entered by `jsr (a0)` into a script or native (Op4_Gosub, Op4_CallNative, the a2 effect-spawn
ops) goes through the type prototype `Op4CodeFn` (every register in/out). Many ops' measured
`ccout(NZVC)` is their own last flag-setting instruction before NEXT (an `add.l`/`move.w` to a5,
a `swap`), read by the caller when the next op returns at once (Op4_Return): those prototypes take
`&ccr` and build it (verify showed SR's Z differing without it, Op4_ChildSetTimerRandom).
Quirks kept: Op4_BranchScreenYOutside's below-lo branch takes its offset from the hi word;
Op4_SaveDrawPos turns by a5's heading, not a6's.

**Waves** ($930C-$939C). A wave director is a one-block a5-VM object (`Wave_Start` $932E,
handler $6A14, script $1E): it waits while g_ObjPopulation >= its cap ($12 one player / $14 two)
and spawns enemies; stage trigger tables (Tbl_015F5A etc., camera-x ordered) start them with
`dc.l Wave_StartFromEntry, script` entries. `Wave_EndAll` bumps g_WaveGen: older waves end.

**a5 wave scripts** (ops $6A72-$6FD2 `Op5_*`, `VM5_*`). `VM5_RunObject` ($6A14) ends the wave
when g_WaveGen > its serial $A; while g_ObjPopulation < cap ($12, or $14 when g_P1Flags and
g_P2Flags bit 0 are both set) it subtracts g_WaveTimerStep ($100/frame) from its timer $2E and at
<= 0 resumes the script at $1E (sp saved in g_VM5SavedSP; ops yield by restoring it). Time words
are 1/16 frame (`Op5_Wait $F0` = 15 frames). Object: +$C spawn count, +$E/$10 quota (1P/2P) and
+$16 the after-quota pc (`Op5_SetQuota`), +$12/$14 caps (`Op5_SetPopulationCap`), +$1A/$1C loop
counters A/B (`Op5_SetLoopA*`/`LoopA`, `...B`), +$26/$28 spawn origin (`Op5_SetOrigin*`,
`Op5_OriginAtScreenCentre`), +$2A interval reloaded into $2E after every spawn op
(`Op5_SetInterval`), +$32 kill group, +$34 return-stack pointer (stack from +$36, `Op5_Gosub` /
`Op5_Return`). Spawn ops `Op5_Spawn` / `SpawnMulti` / `SpawnRandom` take offsets to **spawn
entries**: word x, word y ($E000-range = screen-relative + camera, $2000-range = world, - $2000),
long = spawn action byte + spawner routine (Obj_SpawnEnemy wrappers), spawned by `VM5_SpawnEntry`
($6FEA); each spawn op then yields through `VM5_AfterSpawn` ($7052: interval reload, quota check). Typical script: `Op5_EndOlderWaves`, caps, spawns
and waits, `Op5_WaitPopulation0` (all dead), `Op5_ShowGoSign`, `Op5_End`; arenas use
`Op5_LockCameraHere`. **Kill groups** (`Op5_StartGroup` $6F0A: members, reward action, frame
limit): members get spawn action 1 (`SpawnAction_GroupMemberReward` $6F70: kill them all within
the limit and the reward action, e.g. $1B an item, runs at the last kill); a member removed
without a counted kill goes through `SpawnAction_GroupMemberGone` ($6F9A, from Obj_Destroy).

**a6 loader scripts** (ops $70B8-$772E `Op6_*`, `VM6_*`). One global script at a time (g_A6ScriptPc
$E036) loads a scene's graphics. Scene setups run it at once with `VM6_Run` ($70A4, a6 = script,
39 callers); `Op2_StartA6Script`, `VM6_Start` ($7094) and the trigger callback
`VM6_StartFromTrigger` ($7092, `dc.l $7092, script` in stage trigger tables: section loads while
the stage scrolls) start one for Frame_End instead. Frame_End calls `VM6_RunHook` ($7086): it jumps
to g_A6ResumeHook ($E03A) when set. `VM6_Resume` $70A8 continues the script; ops that wait
install their own hook. NEXT = `VM6_Next` $70B0 (sets g_A6ScriptBusy). `Op6_End` $70B8 clears
it. **Buffers**: word operands are byte offsets into g_BufPtrs ($E092, 16 longs). A buffer starts
with self-relative longs to its entries (word "entry offset" operands). Ops: `Op6_Decompress`
(7108: slot, dc.l LZ block) / `Op6_DecompressBg` (7120: background task, waits for g_BgTaskActive),
`Op6_StoreLzEnd` (slot = g_LzOutEnd), `Op6_SetBufPtrToEntry`, `Op6_InitBankFrames` (sprite bank ->
RAM frame table), `Op6_LoadPalettes{,Yield}` (count byte + mask byte, 2 bits per palette =
line), `Op6_InitLayerAOnPlaneA/B` (Layer_Init on g_LayerA), `Op6_DMAEntry` (7604: entry = word
byte length + data, DMA now) / `Op6_QueueDMAEntry` (7682: through the DMA queue as budget allows,
`VM6_DMAStep`), `Op6_DrawMap{,Rect}` (7468/747C: cell map entry = word w, word h, cells, + tile
word, onto the plane named by a plane-VRAM variable ($EE34/$EE36), written now) and
`...VInt` (7488/749C), `Op6_ClearPlane{,Rect}` (tile $5FC). **V-int jobs**: these ops allocate a
V-int list object ($8BB8; handler `VM6_VIntClearRows` / `VM6_VIntBlitRows` writes rows as the
DMA budget allows), then park. When the last row is written, the end hook frees the object
($8BF6) and resumes. `Op6_MoveLayerAToCamera` + `Op6_RedrawLayerA` (one block column per frame,
`VM6_RedrawStep`), `Op6_CallNative` (dc.l + the native's inline args, e.g. `Section_SetIdInline`
$772E, ArtCache_AddBlocksInline $8580) and `Op6_CallNativeRetry` (re-called each frame by $70D2
until d0 = 0). E.g. Tbl_01FAB2 (stage 1 street): `7108 0000 0005E63C` (unpack into buffer 0),
`7604 0000 0004 4940` (DMA entry 1 to VRAM $4940), `7468 0000 0000 EE34 224A` (map entry 0 onto
plane A). Never-run variants (raw): $70D2, $71E8/$71F6 (layer $EDE8), $7366-$73BC (other camera-axis moves; $73A6+ on $EDE8), $73F6/$73FE (redraws of $EDE8),
$721E/$722E + $7296 (direct-write clears).

**Triggers** ($12DC2-$12ECD). A trigger table = word count, then entries: +0 size, +2 x key,
+4 y key, +6/+8 x/y (d4/d5), +$A callback long whose top byte is a spawn action (stored to
$19 of the spawned object, run at its death by Op2_RunSpawnAction). `Trigger_LoadX` sorts it into
g_TriggerList; `Trigger_RunX` (every gameplay frame) fires entries whose x key is on screen
(g_CameraX..+$140); a callback returning d0 != 0 is retried next frame. `Trigger_LoadY` /
`Trigger_RunY`: the same by y for the vertical zeppelin stage ($032F14).

**Spawn actions and health drops** (Tbl_012EF0, 50 longs; `SpawnAction_Run` $12ED0). Action 0
rts, 1/2 group counters ($6F70/$6F9A), 3-35 item spawners ($1393C-$13B0E, other range), 36-49
`Drop_*` ($12FE4-$132BD): a4 effect scripts (`Drop_RunFxScript`) that fork 1/2/3/5/9 health
pickups of two styles (A child $13252, B child $13220) or a timed row of 5; each pickup plays
sound $12 and calls `Drop_CollectHealth` (+$10 health to the toucher's record, cap $100). 48/49
pick 1-3 at random behind the cooldown g_DropCooldown. Pickup natives find the collecting player
with `Item_FoundBoxPlayer` (g_FoundBox, set by the a2 overlap ops $375A/$3768) / `Item_BoxPlayer`.

**Map layers** ($12572-$12D05; g_LayerA $ED9C). `Layer_Init` reads a level header of 8
self-relative pointers (block map: word w, word h, then block words; block tiles = 2 longs per
2x2-cell block; block table; collision data; animated tiles). Plane geometry per scene:
`Layer_SetPlanes64x64`/`128x32` (g_Plane*, g_ScreenBlocksX/Y = 22x16). Scrolling: the camera
code sets the pending +4/+6, `Layer_ScrollStep` moves up to 16 px a frame and queues the exposed
block column/row (`Layer_QueueColumn`) into g_TileQueue, written to the VDP in the V-int by
`Layer_FlushTileQueueIfIdle` (unrolled `Layer_WriteTiles`). Scene start: `Layer_MoveX/Y` then
`Layer_DrawScreen`. `Layer_AnimateTiles` DMAs animated tile frames.

**Camera** ($93A0-$957A). `Camera_TrackPlayersX` (every gameplay frame from $9BC0) scrolls right
only, toward the players' x (mean of both) or g_CameraLockX when set; `Camera_LockHere` /
`Camera_LockRightEdge` (trigger table entry, d4) lock it for an arena; the GO! sign ($119EA via
`HUD_ShowGoSignCb`, logic $11A1C) clears the lock. `Camera_UpdateView` derives g_ViewX/Y (with
the shake), the sprite offsets, the on-screen origin g_OnScreenLeft/Top and the movement area
g_MoveAreaLeft/Top. Screen shake: `Shake_Start*` ($959C-$9640) kick g_ShakeY, `Shake_Update`
bounces it. `Pal_FlashWhite` ($9584): 4-frame white CRAM flash ($A7F0).

**Parallax line scroll** ($97F0-$9958). `Scroll_RunParallax` (stage frame loops: `lea Tbl_x,a5`)
sets g_ScrollMode $E55A = $C (the V-int then DMAs the line buffers $FFE1D2 / $FFE392 to VRAM
$BC00 / $BC02, stride 4) and runs a command list through `Scroll_NextCmd` (jump table $97FC, word
commands: 0 end, 4 plane A from a layer, $C/$14 plane A/B from g_ViewX/Y, $18 plane B + x offset;
8/$10/$1C/$20 never ran). `Scroll_FillBands`: band table of (height, divisor) words; each band's
lines get -(x / divisor) with x in 16.8 (divisor $100 = 1:1); height $7FFF = rest of screen.

**Scenes and the frame loop** ($99D4-$9DE4). A scene routine = `Scene_InitPlain` (menus,
story, cards) or `Scene_InitStage` (gameplay: + players, terrain, camera; header layout in its
header), then an endless loop `Frame_BeginStage`/`Frame_Begin` ... `Frame_RunWorld` ...
`Frame_End`. `Frame_End` waits for the V-int (the background decompressor runs meanwhile), runs
`Pause_Update` (section-skip code, debug menu $1569C when $F60E), `Rand_Next` and the per-frame
callback long $F604 ($9CE6 none, $9D72 game-over check, end timers). A scene ends by
`Scene_FadeOutExit` -> `SceneCb_WaitFadeExit`, which pops two return addresses (`addq.l #8,a7`)
so the loop returns to its caller; the result word $F608 (0 next, 1 Start / continue, 2 game
over) is set before. Scripts end scenes with the a2 natives `Scene_EndAfterDelay` ($9CE8) /
`Scene_EndAfterDelayWhite` ($9D24). Word writes/compares of $F604 ($9C50, $1569C, $11208) look
meant for $F608: the long write that follows overwrites them (inferred).

**Main program and sections** ($9E02-$A1BE). `Main_Start` (after boot) loops `Main_Attract`
(Tbl_00A152) -> `Main_Title` ($496A2; idle -> story list Tbl_00A136) -> `Main_NewGame` (player
select $49AA6) -> `Main_PlayLoop`. A section id word $F602 (hi byte stage, $0001-$0004 = stage
openings `Stage_Intro1-4`, $1100 ... $4401 the gameplay sections) is advanced by
`Section_RunNext` through an order table (Tbl_009FFC; Tbl_00A05C = retry the same section after a
continue) and dispatched through Tbl_00A0AC (id -> routine). Game over -> continue $4916E ->
`Main_NoContinue` ($494BE); after $4401 -> `Main_Ending`. `Scene_RunList` runs a list of scene
routines while the result stays 0. `Sound_PlayMusic` ($A1BE, current song $F622, ignored while
$F6FE is set during attract/title).

**Pads** ($A3AE-$A43B). `Pad_ReadPort` (3-button, TH toggle; bit 7 Start, 6 A, 5 C, 4 B, 3-0 R L
D U, opposite directions pressed together are dropped), `Pad_Update` (record: held, previous,
pressed, released). Full V-int: `Pad_ReadAll` -> g_Pad1/2, copied to g_PadLive1/2; busy V-int:
`Pad_ReadLive` -> g_PadLive1/2 only.

**Palette, fades and line tint** ($A43C-$AD83). Palette in RAM: g_PalSource $E0D2 (4 lines x $20;
`Pal_LoadLine0-3` copy a line and set its reload flag g_PalNReload), g_PalFaded $E152 (what
CRAM gets). `Fade_Step` (every full V-int) moves g_FadeLevel 2 toward g_FadeTarget every
g_FadeDelay+1 frames; each line's channel level = g_FadeLevel + the line tint g_PalNTint{R,G,B}
($F4C7, 4 bytes per line); colours go through the clamp tables Tbl_00A6A8/A728/A7A8 (component +
level clamped to 0..$E: -$E black, +$E white). Only changed or reloaded lines are rebuilt
(g_PalLineCache $F4D7, flags g_PalNChanged), and `Pal_VIntUpload` DMAs just those lines (15
descriptor sets at Tbl_00A87E), or runs the white flash (g_FlashReq/g_FlashTimer: CRAM $0EEE).
Starters: `Fade_StartIn` (scene start, from +-$E), `Fade_StartOutBlack/White` (target beyond
the extreme tint), `Fade_ToNormal`, `Fade_FlashWhite6/7/3` (2-frame white CRAM, then fade in
from +$E at 1 step per 6/7/3 frames), `Fade_InFromPale6`, `Fade_FlickerTarget` (random walk).
**Line tint** (a second, script-driven level, g_TintLevel $F4C6 -> g_TintTarget): `Tint_Flash*`
(level $A/$C/$E decaying by 2 every 1-6 frames), `Tint_Step`; `Tint_StepLine0/1/3` copy it to a
line's tint, e.g. boss hit flashes on line 3 (the native $1A182 also puts the sprite on palette
3). Unreached helpers at the end of Tbl_00A87E ($A996-$A9F5, no pointer found): flash 1 frame
+ set/clear g_SceneShadowHL, wait for g_FadeTarget == g_FadeLevelVInt, clear g_PalSource and the
fade, clear the tints.

**VDP setup and V-int scroll upload** ($AD84-$AECB). `VDP_LoadRegs` (19 registers from a byte
table), `VDP_ClearVRAM/CRAM/VSRAM/SAT/HScroll`, `VDP_WhiteCRAM`, `VDP_ClearPlanes` (planes A
$C000 / B $E000 filled with blank tile $5FC). `VInt_UploadScroll` (full V-int) / `VInt_BusyScroll`
(busy V-int) jump by g_ScrollMode through `VInt_ScrollModeTbl` ($AE7C, modes 0-$10, then
`VInt_ScrollModeTbl28` for $28-$3C, several ending in the H-int raster setup $B1E0); every mode
ends with VDP reg 1 = $64 (display back on; the V-int starts with $34). `Scroll_SetMode` switches
modes with the V-int held off and H-ints disabled.

**Scroll modes and the H-int raster** ($AECC-$B231). Modes (g_ScrollMode): 0 `VInt_ScrollDisplayOn`
(nothing), 4 `VInt_ScrollCustom` (jmp g_ScrollVIntFn), 8 `VInt_ScrollPlanes` (whole-plane H/V
longs), $C `VInt_ScrollLines` (VSRAM + `Scroll_DMALines`: 224 lines of each plane, stride 4), $10
`...LinePairs` (448 interleaved A/B words from $FFE1D2), $28/$34 line DMA of plane A/B + raster,
$2C lines + 2-cell column V-scroll (`Scroll_DMAColumnsAB`: $FF9000 A, $FF9028 B), $30 plane A
lines, $38 lines + raster, $3C planes + raster; $14-$24 never ran. The busy V-int only turns the
display on, or for raster modes re-arms the H-int on the table already shown (`HInt_Rearm`).
**Raster**: `HInt_InstallVScrollA/B` copy the handler to g_HIntCode and pick VSRAM 0/2; each frame
`HInt_SwapRearm` swaps the double-buffered 224-word line tables ($FF9040/$FF9240: the game fills
the one in g_HIntTblBack) and `HInt_Arm` sets H-int every line with VDP auto-increment 0, so each
handler write is that line's V-scroll; the handler disables itself after 224 lines.
`Spr_MaskLeftColumn` ($F808, scene flag g_SceneFixedSpr) covers screen x 0-15 with 7 solid
sprites while 2-cell column scrolling is in use (TAS 21180-35460, 101700-108300).

**Line fills and span shapes** ($B232-$10169). Unrolled fills entered by `jmp d8(pc,Dn)`:
`Mem_FillRamp` (16.16 ramp d0/d1 -> words), `Mem_FillWords`, `Mem_FillLongs`, `Mem_FillWords2` (two
tables), and for spans `Span_FillRamps` (H ramp to a0 + V ramp to a1), `Span_ClearLines`, the
dithered `Span_FillRampsDither0/1` (H bit 0 = line parity). **Spans**: filled shapes drawn with
line scroll alone: per line the H-scroll (g_HScrollLinesA) puts the left edge and the per-line
V-scroll (the H-int table g_HIntTblBack, mode $38) picks a plane row whose bar is as wide as the
span (row = V + line; "no span" = row 0, V = -line; the plane picture is inferred).
`Span_DrawTriangle(Dither)` (vertices g_TriX0..g_TriY2, sorted by y, clipped),
`Span_BuildCircle` + `Span_DrawDiscDither` (g_CircleSpans; ORs the disc into spans already there):
the stage 2 flying-section boss's translucent beam and bubble (TAS 77185-79303, png f078000);
the dither phase flips with g_FrameParity. Never-reached tails: $C9FA-$CB79 (96 more
Mem_FillWords2 units), $D82C-$DAB3 and $E81A-$EAA1 (36 more Dither units each): counts above
128 / 188 would enter them; they are code, not data (byte pattern), unconverted.

**Random numbers** `Rand_Next` ($995A, state $F5FC word + $F5FE long, seeded at boot, stepped every
frame by `Frame_End`), `Rand_Signed` ($9982, [-d0/2, d0/2)), `Rand_Below` ($998E, [0, d0)).

**Player records and the HUD** ($11038-$11833). g_Player1/g_Player2 ($40 bytes): +0 long score,
+4 health ($100 full), +6 lives, +8 word weapon charge (byte >= $2B = charged shot), +$1A text ptr,
+$1E/+$20 panel-script timer/pointer, +$25 flags (0 in play, 2 right-side HUD, 3 Robin, 6 sliding
out, 7 panel in text mode), +$26 countdown timer, +$28 weapon type 0-2, +$2A weapon level 0-6,
+$3E pad. `HUD_Update` ($112C8, every frame): `Player_GatherStats` (g_PlayersInPlay, max/min/sum
of weapon level, health/16, lives at $F636-$F648, read by a2 ops $39AC/$3CEE), each panel's script
(`HUD_RunPanelScript`, op table Tbl_011764: Halt/Wait/Goto/SlideOut/SlideIn/WaitSlide/ShowText/
ShowStats/ShowCredits, 12-char texts at $10F38: GAME OVER, PRESS START, BOSS TIME, PLEASE WAIT,
CREDIT nn), then one DMA step per frame by g_HUDStep: 4 HUD art $58452 -> VRAM $0280, then a cycle
8 P1 ring ($0520), $C P1 panel ($0760), $10 P2 ring ($0640), $14 P2 panel ($09A0), drawn into
g_HUDTileBuf $EA24. Ring = health segments + lives digit; panel = score + weapon charge bar
(colour by weapon type) or text. Joining: Start + a credit (g_Credits) unless g_JoinLocked
(`HUD_LockJoin` BOSS TIME, both players out).
Panel slides ($11834-$1191B): `HUD_StartSlideOut`/`HUD_StartSlideIn` set +$12 speed / +$16
step (16.16) toward home x +$A -/+ $68; `HUD_StepPanelSlide` moves +$E each frame;
`HUD_DrawPanelSprites` writes the portrait + panel pieces straight into the SAT buffer.
**GO! sign** ($119E4-$11AC9, g_GoSign*): `HUD_StartGoSign` (via `HUD_ShowGoSignCb`) slides it in
from x $168, `HUD_UpdateGoSign` (every frame from HUD_Update) brakes it, clears g_CameraLockX on
arrival (unless g_GoSignKeepLock), waits for the camera to scroll, holds, slides it off right.
**Boss energy meter** ($11ACA-$11D5D): boss scripts call `BossMeter_Show` (op $2CBC; energy = the
boss's $52 into g_BossEnergyMax/g_BossEnergy; a2 ops $3A4A/$3A54 update g_BossEnergy) and
`BossMeter_Defeated`. The meter is a script object (Scr2_011B3E, g_BossMeterObj): a 4x2-tile
ring at top centre with the percentage left (energy x 99 / max) masked into its tiles
(g_BossMeterTiles $EC64, DMA'd every frame), plus `HUD_LockJoin` (BOSS TIME) while it lives.

**Terrain collision** ($10DCA-$10F37). The a2 move op $4CDA calls `Coll_MoveScene`, which jumps
to the scene's g_TerrainFn ($EA1A), or g_TerrainFnAlt ($EA1E) when bit 0 of (a3) is set; both
come from the scene script ($99D4). Contract: in d1/d2 move, d4/d5 box position, d6/d7 masks;
out d4/d5, d3, d6 wall result, d7 floor result (the caller stores +$50/+$52/+$54 of a3 and clears
the velocity on contact). Routines: `Coll_FlatFloor`/`Coll_FlatFloorRest` (g_FloorY, installed by
`Coll_SetFlatFloor`), `Coll_Stage1StreetFloors`, `Coll_HatterArenaFloor`, `Coll_NoTerrain`, the
block-map pair (`Coll_MoveOnBlockMap`), and $82C. **Block map**: plane A's 16x16 blocks
(g_LayerA +$C map, +$14 block table: data offset word + attribute word, +$18 data base, +$2A row
bytes, +$2E/+$30 size in blocks); a block's collision data = solidity mask word, then per pixel
column (16) a solid [top, bottom] byte pair. `Coll_MoveBlockMap` ($11D5E, (a3) bit 0 clear: airborne, inferred) steps x and y pixel
by pixel (moving up only mask bit 0 blocks: one-way platforms); `Coll_WalkBlockMap` ($120A2, bit 0 set:
grounded) walks x following the floor up/down one pixel per step, then probes below. d3 =
g_CollBlockAttr (the block's attribute word).

**Weapons and hits** ($107E8-$10D99). Player record +$28 weapon type (0-2), +$2A level (0-6),
+$8 charge. `Weapon_Fire` (native op from the player scripts) spawns Tbl_010AF8[type][level]
[charged] (a4 shot scripts $5034A-$504F0; `Weapon_FireFlying` uses the second half in the stage 2
flying section, g_PlayMode bit 1) on g_SpawnCtx with owner id pad+2. `Weapon_KeepShotArt` keeps
the current shot frame's art loaded (Tbl_010A1E, g_ShotArtSlots). Collision boxes made by the a2
op $3784 carry a hit callback long at +$30 and its arguments at +$34 (damage) / +$36 (knockback):
`PlayerAtk_HitPause`/`PlayerAtk_HitLongPause` -> `PlayerAtk_ApplyHit` -> `Hitbox_SpawnAtContact`
(spark at the overlap centre, `Hitbox_OverlapCenter`). Enemy attacks test `Hitbox_CanHitPlayer`
(owner id 1/2, not invulnerable: $16 of the player's a5 block, $78 after spawn). Shots hit with
`Shot_DamageTarget` (a4 native, branches on the target's energy).

**Player objects and enemy hits** ($101A8-$107E7). A player slot object runs `Player_JoinOrSpawn`
(join with Start + a credit; spawn Scr2_04ACD0 Batman / Scr2_04D80C Robin at the camera + the join
offset) then `Player_Run` (pad -> script var 0, releases -> var 1 bit $20, VM2_RunObject, charge
+= Tbl_WeaponChargeRate). Script natives: `Player_TakeHit` (switch: 0 none, 1 knockback, 2
knockdown, 3 dead; hit type in the struck box +$1A: 6 flinch/30 frames invulnerable, 7/8
knockback 40/60, 9 knockdown 60), `Player_LoseLife` (0 playing, 1 out), `Player_HurtFixed` (-$20),
`Player_DespawnNative`; `Section_StartExit(N)` (g_ExitTimer, var 1 bit $800, SceneCb_
WaitPlayersExit). Enemy box callbacks `EnemyAtk_*` test `Hitbox_CanHitTarget` (owner 1/2 or 5;
id 5's owner unknown) or `Hitbox_CanHitPlayer`, add the damage to the struck box +$20, set +$1A
and the knockback +$22, and spawn a spark: touch damage $12(a6) (`HitTouch`), knockdown 3.0/4.0,
edge-aware knockback (`HitKnock`, `HitKnockSpeed`: near the screen edges pushed inward x1.25).

**Text-map menus** ($1438A-$145FB). Menu and text screens (options, title texts, continue, ending)
build a 40x28-word plane image in g_SceneWorkBuf ($FF9000) and DMA it to plane A every frame
(`TextMap_Upload`; `TextMap_UploadRows` for other planes). `TextMap_LoadFont` DMAs the font
($58AA2, 132 tiles) and sets the font map Tbl_0586F2 + tile base; `TextMap_Clear` blanks the image
(tile $5FC). `TextMap_Run` interprets a word-command list (a0; `TextMap_RunInline` = a2 native with
a dc.l): offsets from $143AA: 0 End, $E Goto, $12 Return, $22 SetPos (bytes col*2, row*2), $58 Text
(count byte + ASCII), $96/$A0 Number (+1 / as is, no leading zeros), $EA MenuItem (gosub normal or
blink-highlighted sub-list for g_MenuCursor), $108 BlinkGoto, $118 Typewriter (one character per
carry of a speed accumulator; the caller reruns TextMap_Run while g_TextMapTyping). Menus:
`Menu_MoveCursor` (Up/Down), `Menu_Update` (Left/Right step a setting with wrap; options music/sound
test numbers $F60A/$F60C). `Text_DrawSpritesVertical` ($1416A) = the player-select SELECT/PLAYER
columns.

**Direction math** ($14EC6-$15045). `Mot_Atan2` (d0 dx, d1 dy -> d2 heading 0-$3FF, 0 = +x, $100 =
down; octant-linear), `Mot_DirToPlayer` (one of d4 compass directions toward the targeted player
record, 0 = up, clockwise), `Mot_StepDirToward` (one step round a ring of d1 directions).
**3D points** ($153CE-$155A7): a 3D record (angles +0/+4/+8, translation +$C/+$10/+$14, matrix
+$18-$28; e.g. $F3C4/$F3EE in the $1E9A4 objects): `Rot3D_BuildMatrix`, `Rot3D_TransformPoint`,
`Rot3D_ProjectPoint` (divides by z/4096).
**Debug pause** ($15674-$15A33, never run in the corpus): `Debug_PauseMenu` (A slow motion, B free
camera, C end section), `Debug_FreeCamStep` (cursor g_DebugCursorX/Y, A = move with terrain
collision, C = camera lock, hex readouts by `Debug_DrawHexBytes` with digit frames 1-16).
`Frame_BeginStageHook` ($15674) is an empty rts.

**Stage 1 street** (scene $1FB3A, $159B4-$15F58, plane B 128x32). Facade x = g_ViewX * 35/32 in
$168-px segments (`Stage1_UpdateStreetScroll`); `Stage1_ScrollFacadeLines` shears plane B's line
scroll in 5 bands. Four sign slots ($F4FA-$F4FD wanted, $F4FE-$F501 drawn: 0 intact, 8 blasted;
$20866 blasts one) are redrawn by the V-int object `Stage1_VIntFacade` / `Stage1_DrawSignPanel`
(12x7 tiles, Tbl_015CF4). The building fire: trigger callback `Stage1_StartFireAt` -> state
g_Stage1FireState 2 (blank plane A top) -> 1 (`Stage1_VIntFireStep` cycles Tbl_015A7E frames,
`Stage1_SwayFireLines` sways plane A); before it `Stage1_WaveSkyLines` ripples plane A's top band.
$F4F4-$F508 are per-scene scratch reused by other scenes ($1C3F2, $2417E, $3BB88, $47674 ...): they
are written `g_SceneVars+offset` (one generic name; never give them scene-specific global names). **Harley mid-boss** ($1805A-$1A34D, scripts $18066-$1A04B): `Boss_Harley_Spawn`
(last street trigger) -> a rolling cart with a balloon cover (rumble shakes, 2-frame roll, a
48-step aim $22/$24(a5) with muzzle table Tbl_01A28E) that later raises Harley on a telescoping
pole (pole block at $10(a6): +0 target, +4 height, +8 velocity, +$C accel; `Boss_Harley_StepPole`
and the `PlaceAtPole*` segment natives); the native $1A34E launches arcs (Scr4_01AB76) from the pole top at the targeted player (black balls beside her at f5850: bombs, inferred); it is `Boss_Harley_LobAtPlayer`.

**Stage 1 enemies** ($1ACE0-$1C017). Each family = short spawner prologues (`lea Scr2_x / jmp
Obj_SpawnEnemy`: ModSprAttr and/or bytes, $98/$60 = palette 3, $98/$40 = palette 2; score; energy;
the entry coroutine 0; vector 2 = the shared body) in front of one shared body. **Brute** (purple
strongman with a sack, body $1AF74 in Scr2_01AF1A): `Enemy_Brute_Spawn{FromSign,,Tough,ToughAlt,
Phased}` (FromSign = leaps out of a sign blasted by `Stage1_BlastSignChained`). **Clown** (striped
shirt, pointed hat; body in Scr2_01B7CC): `Enemy_Clown_Spawn{,Weak,Green,RunIn}`; most drop in from
above the screen (they wait at y < 0; op $1CDC = Op2_FallAnim without its 1-px nudge). **Gunman**
(trench coat, pistol; Scr2_01BFC4): `Enemy_Gunman_Spawn{WalkIn,DropIn}`. Never run, still raw: a
spawner for Scr2_01B760 at $1B754 inside Scr2_01B706.

**Warehouse bosses** ($1E70A-$1F0DB, section $1301). `Boss_Claw_Spawn` (trigger): Harley's crane
claw, a 3D object: a 12-ball chain (point list Scr4_01EC0E, labelled a4 script but it is 10-byte
points) and three 3-ball fingers (Scr4_01EC86 head) drawn as depth-scaled ball sprites
(`Boss_Claw_Draw` -> `DrawChain`/`DrawPoints`, frames by size/depth from Tbl_01EBCE) through two 3D
records: g_Rot3DOuter (perspective, the whole claw) and g_Rot3DInner (finger spin, `PlaceFingers`).
Swing modes are slot-2 natives (`SwingSideways`, `SwingAtCrane` = parallax at the background crane,
`SwingFingers`) over a pendulum (`SetSwing`/`StepSwing`); it can hit only at depth $700-$800
(`Boss_Claw_Draw` returns that to an op $2D30 switch). Jetpack Harley (Scr2_01EE64):
`Boss_HarleyJet_Spawn` (trigger), `...SpawnSecond`/`...SpawnFinal` (from the claw script; the final
one has the boss meter); her defeat sets g_SceneVars byte 0, which the claw tests
(`Boss_Claw_BranchHarleyBeaten`). **Joker balloon** (truck end, `Boss_JokerBalloon_Spawn`): the
balloon is a plane-A picture; a V-int list object (`Boss_JokerBalloon_VIntSplit`) scrolls plane A
to the boss's x/y above its bottom line and an H-int (`...HIntRestore`, g_HIntCode jmp stub) puts
the stage's plane-A scroll back below it. `Stage1_SpawnPoliceCar`: the cruiser beside the truck.
**Propeller drones** (`Enemy_Copter_Spawn{Red,RedFast,Green,GreenFast}`, $1F480-$1FA35): wave-script
spawners (Obj_SpawnScriptedOrQuit) that take a delay word from the wave entry (a1) into loop
counter 0: the drone flies straight that long, then homes on a player and blows up on contact.
Section routines: `Stage1_RunStreet` $1100, `Stage1_RunGemExpo` $1300, `Stage1_RunWarehouse` $1301,
`Stage1_RunTruck` $1400 (refills both players' health at $1300/$1400). Unexplained:
`unk_Stage1_01FF78` (street trigger at the two bank signs; an a4 object showing RAM frame $FC40).

**Stage 1 sections and their objects** ($1FFA6-$20E1B). Stage 1 = four scenes: $1FB3A street
(TAS f503), $1FC50 Gem Expo interior (f6334), $1FD76 warehouse (f9615), $1FECC parade truck
(f13217); stage 2 starts with the vertical zeppelin $32F14 (f21125). Breakables (energy $20, one
frame, drop = the trigger entry's top byte): `Stage1_SpawnTrashCan` (street), `Stage1_SpawnUrn`
(Gem Expo), `Stage1_SpawnGiftBox` (truck). **Bank bombing** (street trigger
`Stage1_SpawnBankBombing`, f1729, director Scr2_020CF6): waits for no enemies, then explosion row,
`Stage1_BombingBlastLeft/RightSign` (-> `Stage1_BlastSignAt`: slot state 8 + `Stage1_SpawnSignDebris`,
18 tumbling pieces), `Stage1_StartRubbleRain` ($578 frames of bouncing rubble on g_TerrainFn),
big blast, `Stage1_StartFireAt`, `Stage1_StartFireFlicker` (palette line 1 toggles between the two
palettes at $020E1C: labelled Scr4_ but it is palette data), then the wave in the trigger entry's
extra long. Trigger entries can carry such extra longs after the callback (a1 points at them):
`Stage1_BlastSignChained` pushes one and "returns" into it. **Balloon bombs** (truck): launchers
`Stage1_SpawnBalloonLauncherA/B` (wave script Tbl_0174F0) sit at the right screen edge and call
`Stage1_LaunchBalloonA/B` (next (height1, height2) pair from Tbl_020354 / Tbl_02036E, a negative
word wraps); balloons Scr2_0203E0 bob between the two screen heights (`Stage1_BalloonBob`).
Never-run launcher code still raw: $02028E (start table $020388) and $020316 (LaunchBalloon wrapping
to $020388; called from Scr2_020094 at $02024A-$02025E, a part of the script the corpus never reached).

**Stage 2 lift and boss arena** ($0240EE-$0247F8 and $024AD6-$025563; scene `Stage2_RunLift` $32F14, section $2100). The
zeppelin loading dock is a lift: the camera stays at (0, 0) and the map scrolls (g_MapScrollY
from $1910 down to 0). `Stage2_LiftStart` / `Stage2_LiftStop` (y-trigger callbacks in Tbl_02105A)
set the goal speed $DFFE; `Stage2_LiftUpdate` eases g_LiftVelY to it (jolt on stopping). Objects on
the girders move with `Stage2_MoveOnScrollingMap` (block map at y + g_MapScrollY). Scroll:
`Stage2_LiftVIntScroll` (V-int object, kept in $F5C2) DMAs the shaft's column V-scroll
(`Stage2_LiftColumnScroll`, depth bands Tbl_024642) and arms the H-int stub `Stage2_LiftHIntFloor`
at line $AF - shake: below it the lift platform uses a fixed scroll. At the top (f32065)
`Stage2_LiftArriveTop` makes it the boss arena (g_SceneVars = 1): a **tilting floor** drawn with
scroll alone (`Stage2_ArenaDrawTilt`: slope g_SceneVars+6 -> target +8 = +-$1800, speed +2, pivot x
$F5A6 -> $F59E, floor y $F5AA; plane B shows plane A's name table below the split line $F5BC via
`Stage2_ArenaHIntSplit`) with terrain `Coll_Stage2TiltFloor`/`...Rest`. The boss script asks for a
tilt with `Stage2_ArenaRequestTilt` (4-frame rumble, then the slope eases over with debris effects
Scr4_024360 and a big shake on arrival). Enemies: green-suited thugs, one body: with a barrel
(`Enemy_BarrelThug_SpawnA-F`, entry $024CC8; `Enemy_BarrelThug_ThrowBarrel` throws it at the
targeted player and, on death, drops it carrying the thug's drop) or without (`Enemy_GreenThug_
SpawnA/B`, entry $024CD8); variants differ in timers and drop, `Stage2_SpawnLeapingThug`
(+ $0255B2, both end in `Stage2_SetSpawnVarsFromEntry`: script vars 0/1 from the entry).
$F59E-$F5DD are a second per-scene scratch block, written `g_SceneVars2+offset` (also used with
other meanings by $3CEF8, $3DA2E-$3DE62, $4062A, $40980); g_SceneVars itself runs on past $F508 in
some scenes (arena column tables +$32/+$5A/+$82, corridor bands +$12..+$91).
**Flying-section corridor** ($024824-$024AD5; scene $33274): `Flight_UpdateCorridor` +
`Flight_DrawCorridor` draw the perspective ceiling/floor and the banded middle wall into plane B's
line H-scroll and the H-int V-scroll table (spans, see "Line fills and span shapes").
**Flying-section scene and checkpoints** ($03324A-$033BAB). One scene, `Flight_RunScene` (the
loop adds 1.5 px a frame to g_MapScrollX and calls the frame callback g_SceneVars+$98), entered
by four section routines that push their own a6 loader script: `Stage2_RunFlight` ($2200),
`...Clouds` ($2300), `...Airship` ($2400), `...Boss` ($2401). The $2200 loader (Tbl_03301E, really
four a6 scripts, the later ones started from wave scripts / the hull script $030FE2) sets the
section id at each checkpoint (`70E0 0000772E id`) so a continue resumes there; the others are the
same segments shortened. Parts (TAS): corridor f35416 (`Flight_StartCorridor`,
`Flight_FrameCorridor`), cloud bank f46290 (`Flight_StartClouds`: plane A clouds scroll 1 px a
frame with a sine line wobble, `Flight_ScrollCloudBank`; the loader waits in
`Flight_WaitCloudsPassed` until layer x $E40, f49938), sky only (`Flight_DrawSky`: plane B, two
interleaved cloud layers at -7/-3 px a frame swapped on alternate frames), airship hull f67262
(`Flight_StartAirship`/`FrameAirship`, plane A map at camera + g_SceneVars+$94/$96), beam ship
f77184 (`Flight_StartBeamShip`/`FrameBeamShip`). g_SceneVars use here: +$94/+$96 hull map offset,
+$98 frame callback, +$9C sky drift long, +$A0 wobble phase, +$A2 beam-ship death-flash flag, +$A3
cloud bank on screen; the wobble table is g_SceneWorkBuf+$400 (480 words).
`Stage2_SpawnLiftLamp` (lift y-triggers): breakable glowing lamps on the lift shaft.
**Flight-section bosses** ($028D70-$029882). `Boss_TurretPod_*` (TAS 45122-46053): a red pod with
three twin-barrel turrets; each turret is an unlinked child in pod fields $1E/$22/$26 (spawned by
`..._SpawnTurret1-3`, var 0 = bit 1/2/4) that `Boss_TurretPod_RunTurrets` places at pod + its a3
+$14/$16 and runs inline every frame; the pod's var 0 = OR of the live turrets' bits, and its own
body box appears only when it is 0. `Boss_BeamShip_*` (the final ship, scene init $33A62, TAS
77184-79108): the ship (script $2DF94/Scr2_02928A) owns an unlinked **radius object** (Scr2_029598)
whose x eases toward the ship's var 0 and is copied to g_BeamRadius; the beam spot lives in the
ship's saved position $46/$4A, moved by coroutine 1 with a second motion block at $E(a6)
(`BeamOn`: radius + beam colour into palette line 1 colour 15, spot homes on the player; `BeamOff`:
radius 0, spot returns). The scene callback $33A88 draws a dithered span cone from g_TriX0/Y0 (ship
- $18) to a disc of g_BeamRadius at g_TriX1/Y1 (spot); g_SceneVars+$A2 set = disc only (the death
flash: the radius object takes over, flashes 7 times, shrinks and calls Section_StartExit).
**Flight enemies** (bodies in $027DBE-$0285DE, spawners $0299D2-$02ABC2; wave entries carry a
spawn-action byte in the spawner long's top byte): black drone (body $27DBE, energy $68),
red drone ($27F70, energy $88), red biplane ($28286, `Enemy_Biplane_*` anim natives), spinning
mine ($28468), rocket jet ($285DE, fires), homing missile (Scr2_0287B0 via
`Enemy_Missile_InitFromArgs`: heading/straight time/homing time from g_VM2Arg0-2). Identified by
rendering their frames from Tbl_1DDB60 with the RAM palette.
**Stage 2 pole thugs, ladder thugs and the arena boss** ($0255B2-$028301). **Pole thugs**
(`Enemy_PoleThug_SpawnA-D`, lift y-triggers; `Stage2_SpawnLeapingThug` $025560 is the same family:
all share the body $02577E) slide down the left/right side pipes (entry x $28/$118) with sparks
(`Enemy_PoleThug_SlideSparks`), hold and shoot per variant, and jump off at the entry's var 1 y
(`Enemy_PoleThug_CheckDropY`). **Ladder thugs** (`Enemy_LadderThug_SpawnA-C`, 6 blocks; +$10(a6) =
ladder block) hang on a swinging rope ladder (`Enemy_LadderThug_SwingLadder`: pendulum, 10 segment
sprites, the thug drawn at segment 5) and throw bombs in arcs (`Enemy_LadderThug_Throw` runs the a4
script held in coroutine slot 3). During the boss they hang from its hull (`..._PickAnchor` on
g_Stage2BossPlaneOn; hull-space position in vars 1/2, `..._PlaceOnHull`). **Arena boss**
(`Boss_Stage2_Spawn` from Stage2_LiftArriveTop, f32065): the machine hull above the arena is plane
B's picture (position g_SceneVars2+$16/$1A = the boss's x/y, `Boss_Stage2_MoveHullRunTurrets`),
with four unlinked turret objects (`Boss_Stage2_SpawnTurret1-4`, a6 +$1E-$24, run inline by the
boss; `Boss_Stage2_SetTurretNCoroutine` let the hull's attack scripts make them fire). Hull points
follow the floor tilt via `Boss_Stage2_TiltPoint`. Phase 1: turrets (energy $800/$1000 each, added
to the meter); each one destroyed sets its bit in g_Stage2TurretsDown; at $F phase 2 (f33171): the
hull itself is hit, waits for a floor turn, drops lower and then forces a tilt every $690 frames
(player-requested tilts end, `Boss_Stage2_EndPlayerTilts`). Defeat (f33605): escalating random
explosions (`Boss_Stage2_RampExplosions`/`TickExplosions`, 720 frames once no enemy is left), white
flash and the wreck picture (`Boss_Stage2_ShowWreck`, f34683), `Boss_Stage2_SpawnEndLadder`
(Section_StartExitN after $B40 frames) -> the flying section (f35416). Which villain the hull
belongs to is not shown in the code (Two-Face per the stage, inferred). Flying-section spawner tails
`Spawn_SetTimer0/1FromEntry` set the new object's timer 0/1 from the entry word; gunner
animations fire on script var 0 requests (`Enemy_BlackDrone_NextMuzzle`; body $0280EC = the dome
craft, see "More flight spawners": `Enemy_DomeCraft_TakeFireRequest`).
Never-run code still raw: ladder-thug spawners $02787A (Scr2_02788E) and $027896 (Scr2_0278AA)
inside the Scr2_027872 span; pole-thug variant spawner $02570C (Scr2_02571C; triggers $0217C8,
$021804, $021ABA, $021AF6 exist but the corpus never reached them).
**Stage 3 enemies** ($035740-$036733; sections $3100 film-set backlot f79360, $3200 mushroom forest
f92044, $3400 card hall f111166). **Bomb rabbits** (grey rabbits carrying a bomb ball, body
$035DA2, energy $80): `Enemy_BombRabbit_Spawn{DropIn,Hop,HopPop4,HopFar}`; touching a player (or
timer 2 after $F00 ticks) lights the fuse: `Enemy_BombRabbit_TickFuse` speeds up the bomb's blink
(`...FuseBlink` picks frame $4718/$4722), then the blast $3634C; `...SetBlastGap` sets g_ScriptTimer
= 20 and `...BlastRecent` makes the other lit rabbits wait for it (one blast at a time). **Bomb
dropper** (`Enemy_BombDropper_Spawn`, trigger table Tbl_033CBE, on the overhead pipe at y 19) drops
**bouncing bombs** (`Enemy_BombDropper_DropBomb` -> `Enemy_BouncingBomb_Spawn`, boss class) aimed at
the targeted player; a player's hit bats a bomb back (`..._TakeKnockback`). **Puff plants**
(`Enemy_PuffPlant_SpawnA-G`, Tbl_033F30, invulnerable body $0355D4) puff columns of rising white
balls (three a4 puff scripts $356EE/$35714/$3573A) whose touch throws the player upward
(`Enemy_PuffPlant_PuffHit`: stun $A, y speed -4). **Rabbit pot** (`Enemy_RabbitPot_SpawnA-D`, energy
$180, body Scr4_03575E which is an a2 script): a striped pot at screen y $73 that puffs shots
(a4 $3596E) and lets out bomb rabbits through Spawn_AtOwnOfs. Never-run spawners, still raw:
puff plants $0364E0, $0365D6, $0365FC; rabbit pots $036628, $0366C0; $036934/$036968 (Scr2_036940/
Scr2_036974, the body $035AA6 that $03699C also uses).
Flight-enemy spawners in $0299D2-$02ABC2: black/red drone spawners
(`Enemy_BlackDrone_Spawn*`, `Enemy_RedDrone_Spawn*`) differ only in their course (waypoints,
player-relative hover windows via $27D34, or TrackY = follow the player's height via $27D1A) and
attack loop; they end in Spawn_SetTimer1FromEntry (timer 1 = the wave entry's word = delay before the attack).
Drone var 0 is a shot counter: Enemy_BlackDrone_NextMuzzle fires one shot per change, odd = upper gun,
even = lower. Never-run spawners still raw inside the Scr2_02A008 span: $02A072 and $02A0DC
(`lea Scr2_02A082/Scr2_02A0EC`, referenced by Scr5_023B38 +$96/+$A0, a part no run reaches). The
span labelled Scr4_0298AE also holds a2 subroutines: $2999E / $299B8 launch a spinning mine
(`Enemy_SpinMine_SpawnDown/Up`) at the object unless a player box is within $1E px.
**More flight spawners** ($02ACE0-$02F05A): drones `Enemy_RedDrone_Spawn{CircuitB,HoverOnPlayer,
Loops,DropA/B,RiseA/B,HoverAbove/Level/Below}`, `Enemy_BlackDrone_Spawn{CircuitA/B,HoverOnPlayer}`,
`Enemy_Jet_SpawnLane` (5 screen lanes, Tbl_023764); **gunships** (`Enemy_Gunship_SpawnA/B`, body
$2800A: energy $A00, a large grey ship, f52500/f65900) launch homing missiles through a2
subroutines that call Spawn_AtOwnOfs with `Enemy_Missile_SpawnGunship{A-D,Fast}`; the subroutines
take 3 args via the unlabelled interior entry $2D04 of Op2_GosubArgs (Arg2 homing time, Arg1
straight time, Arg0 heading); **dome craft** (`Enemy_DomeCraft_Spawn{FromLeft,A,B}`): the body
$0280EC is a green-domed craft with a vertical bar and thrusters (f44400, f65000), shoots fans or
shot bursts and launches spinning mines; **biplanes** (`Enemy_Biplane_Spawn*`, body $28286, no
attack: kill-group zigzag/weave courses in Scr5_023C78); the turret pod `Boss_TurretPod_Spawn`
and its turret creators `..._CreateTurret1/2` (the third is $2F07C). Script motion args: the
waypoint op `Op2_SetVector2HereGotoArgs2` takes the words **(y, x)** (first word -> g_VM2Arg1 =
y, second -> Arg0 = x, Op2_AimAtTargetArgOffset), and `...Args4` window words are (y far, y near,
x far, x near) for Op2_SetPlayerWindowFromArgs; e.g. `0000 FFD8` is 40 px left, not up.
The waypoint subroutine the Args2 offset jumps to decides the meaning: $27D90/$27D98 =
Op2_AimAtTargetArgOffset (relative to $14/$16, set by Op2_TargetSelf), but $27DA0/$27DA8 =
Op2_AimAtArgPoint (an ABSOLUTE x/y): the circuit corners (x $3C/$50/$78, y $50/$13C), WaypointsA/B
pairs 2 and 5 and the Loops point (x $D2, y $C6); the vertical mirrors reflect absolute y about $C6.
Window bounds pack to bytes of half the value (Mot_StorePlayerWindow), so x far $10E wraps to
-$F2: in the "+$F0..+$10E" windows the drone hunts round x +$F0 instead of holding a band.
Never-run spawners still raw inside script spans: $02CFF2 (`lea Scr2_02D002` missile, from the
gunship subroutine $2CF38), $02D406 (`lea Scr2_02D414`, a third dome-craft course) and $02DF4C
(`lea Scr2_02DF58`, a biplane).
**Airship hull objects** ($02F07C-$032D5A; TAS 67262-77147, png f067800-f076800). The hull part
scrolls by a **path object** (`Flight_SpawnAirshipPathObj` at the scene start, parked in
g_SceneVars+$92 by `Flight_AirshipPathObjPark`; `Flight_AirshipStartPath` gives it Scr2_02F644:
cruise x $AA right + a Y oscillator) whose x/y, published every frame by
`Flight_AirshipSetMapOrigin`, are the map offset g_SceneVars+$94/$96 (map = world + offset).
`Flight_AirshipRunTriggers` fires Tbl_0307C2 (130 entries, map coordinates) and stores each new
object's map position in its $4E/$50; **map-anchored** bodies call `Flight_AirshipMapToWorld`
every frame (world = map - offset); movers use `Flight_AirshipLoadMapPos` / `...StoreMapPos`
around their move ops. Objects (frames rendered from Tbl_1DDB60): hull lights (no box, body
$2F746: `Flight_SpawnStrobeLight`, `...RedNavLight{,Flip}`, `...GreenNavLight`,
`...Searchlight`), hull gun turrets (body $2F98C, `Enemy_HullTurret_Spawn{Aiming,Spinning,
Delayed}`), missile domes (hatch opens, a rack launches homing missiles: bodies $2FC42/$2FC56/
$2FC6C, `Enemy_MissileDome_Spawn{Singles,Fan}`), ball turrets (body $2FDD4/$2FDE4,
`Enemy_BallTurret_Spawn{Volley,Stream}`), pipe cannons sliding on vertical pipes (Scr2_030416,
`Enemy_PipeCannon_Spawn{3Way,3Then5Way,5Way}`; first kill = broken and sliding down, second =
explodes) and the **dome-cannon mid-boss** (`Boss_DomeCannon_Spawn{TwoDomes,DomesTurrets,
FourDomes}`, f70036/f72564/f75007: an invisible parent body $3050A runs unlinked children inline
and dies with the last: the core $300EE, energy $1400, and missile domes / ball turrets; if the
fight lasts $1A40 the core shows the boss meter). Missile spawners (all Scr2_0287B0 +
`Enemy_Missile_InitFromArgs`): `Enemy_Missile_SpawnAgile/Slow/Fast` (domes, through the a2
subroutines $3164E/$316CE/$31710) and `Enemy_Missile_SpawnFromPodA/B` (turret-pod attack
subroutines $2F36A-$2F512, reached only in fuzz-45000 runs). Never-run spawners still raw
inside script spans: $02F6B6 / $02F716 (H-flipped strobe / green light), $0311C6, $03129A,
$03135C (hull-turret variants; Scr2_031068 is an unrun turret script), $03169E and $031760
(missiles); Scr4_031452, Scr2_031FC4 and Scr2_03239E are never run.

**Pickups** ($132BE-$13D2A; spawn actions 3-35 of Tbl_012EF0). Each pickup kind has three
spawners (`lea Scr2_x / jmp Obj_SpawnEnemy`) differing only in the motion coroutine (slot 1):
plain `Item_SpawnX` ($133AE, dropped where the object died), `...FromHit` ($13378, pops up;
`Item_LaunchFromHit` gives it half the struck box's knockback +$22 as x velocity), `...Drifting`
($133CA, cruise x $AA, body $13316: the flying sections). Common body $132D0: lifetime timers,
off-screen kill. A touch coroutine (slot 2) plays a sound and calls the collect native (op $2CB2):
actions 21-23 rapid charge (player +$26 = $2D0 frames of $280/frame charge, `Item_CollectRapidCharge`),
24-26 bomb (`Item_CollectBomb`: g_ScreenClearTimer + Hitbox_DamageAllEnemies $200, flash, shake),
27-29 extra life, 30-32 large health (+$80), 33-35 small health (+$10; spawners $134DE-$1350A and
native $13540 still raw, never run). Actions 3-20 are weapon pickups (6 value tables x 3 motions):
`Item_WeaponItemInit` picks a value (0 = level down, 1-3 = weapon type 0-2) from tables
$13C5C-$13C7E, `Item_CollectWeapon` switches type or raises the level (max 6); icon by value via
op $2D30 + `Item_WeaponItemValue` ($13D2A- anim scripts). g_WeaponItemCount counts them.
**Weapon-drop director** ($13EF0-$1407A): a singleton boss-class object (g_WeaponDropper) whose
coroutine 0 runs a command script given by `WeaponDrop_Command` (stage trigger tables) or
`WeaponDrop_CommandNative` (op $2CBC); $13F34 = stop. Its scripts ($13FC4-$14076) wait a random
time, wait while a pickup is out (`WeaponDrop_ItemsOut`), check g_WeaponLvlMax (op $3CEE), then
drop a level-up power-up (`Item_SpawnPowerUpFalling` from the top, `...Drifting` from the right;
collect native $13D18, raw: level +1). **Sprite text** ($1407A-$141B2): `Text_DrawAtObject` (op
$2CBC + string) / `Text_DrawSprites` draw \r-terminated strings (SELECT, PLAYER, GAME OVER, the
stage cards LEVEL ONE / HAPPY BIRTHDAY TO ME! ...) as one sprite per letter (Tbl_0141B2,
g_TextGlyphFrames).
**Stage 3 toy figures, chains and rings** ($036CD4-$037BC3, scripts to $0398FE; TAS 79,700-113,000).
One sprite, a toy figure with arms out (frames $4344-$43DA), drawn at 60 rotations through
Tbl_036DA8 (frame word, attr AND, flip XOR); the first $F0 bytes of "Scr4_036E98" are the same
table for the outlined (charged) frames $43E4-$447A (the label is wrong there; the a4 script
follows). Script var 0 bit 2 = **charged** (outlined; touch damage on), bit 3 = warning blink;
the coroutine-3 schedules $0380B8-$038138 / $0387DA-$038878 toggle them (vector 1 -> $038A90
blink then charge, $038AA4 blink then calm). **Single figures** (`ToyFigure_*`, body $036BCE,
energy $10) hop and tumble (`ToyFigure_DrawByVelY`). **Chains** (`ToyChain_*`): a base object
(Scr2_0379BC loop, e.g. Scr2_0397A6) holds an unlinked chain child made by
`ToyChain_SpawnInline` (46 jsr sites to $03B0EE; dc.w links, dc.l table, dc.l script) with a
chain block at $E(a6); the links are pool blocks used as collision boxes ($34+ of the block,
count a4 +$24), grown 20 px apart (`ToyChain_Grow`), drawn along a bending heading driven by
the chain's own x (a sway value, not a position: the motion ops cruise it toward the player),
shed one by one as single figures by the coroutine-0 schedules Scr2_0386CC ($03887C...:
`ToyChain_RemoveLastLink` + Spawn_AtAnchor). **Rings** (`ToyRing_*`, body Scr2_037588,
5-block spawners `Obj_SpawnEnemyN`): a hoop of N figures that rolls with its x speed, squashes
on landing (a damped spring) and breaks up into single figures (`ToyRing_StartBreakUp`).
g_FrameCounter's low word ($E030) is read as `g_FrameCounter+2` for the blink.
Spawners: `ToyChain_Spawn5/6/7/8` (charge cycle $0380B8, random shed delay), `..._Spawn5B/6B/7B`
($0380D8, fixed delay $03870C), `..._Spawn10A/B` (10 links); chain children `ToyChain_MakeChild5/6`
(+ $0398FE-$039AA6 in the next range); `ToyRing_SpawnBig` (16 figures, energy $400),
`...SpawnSix`, `...SpawnRandom`/`RandomAlt` (4-7 figures, gosub $0392CC); `ToyFigure_Spawn`
(+ `Charging`, `StopHop`, `RandCharge` variants). **Drop carriers** (`Enemy_DropCarrier_Spawn*`,
shared body $035AA6, other range): fly across at screen y $84 and drop white-rabbit hoppers
($035CF2, seen at f111900) or toy figures. Never run at 4:3, converted 2026-10-30 because
widescreen=16:9-gameplay reaches them or their wave (ws-fix; each named by a static caller, checked
by a .syn case at its sibling's first call): `ToyRing_SpawnSixB` $0391AA, `ToyRing_SpawnSixC`
$039290, `ToyRing_SpawnBigB` $039018 (wave spawn entries), `ToyFigure_SpawnB`-`G` $038AF8,
$038B80, $038BBC, $038BDA, $038CF4, $038D48 (Op2_CallNative Spawn_AtAnchor / Spawn_AtOwnOfs
operands in shedding and ring break-up scripts). **Converted too (ws-tail, 2026-10-30)**: the five
chain bases the wave spawn entries name and the two raw makers their scripts spawn:
`ToyChain_Spawn10C` $0396E6 (entry $034912, x $14A; ToyChain_MakeChild10, charge $038578, shedding
$038788), `ToyChain_Spawn10D` $03970A (entry $034686, x $154; Scr2_039716 = 10C's words, its own
copy), `ToyChain_Spawn10E` $03972E (entry $03468E, x -$14; charge $038596, shedding $0387CC),
`ToyChain_Spawn7C` $039776 (entries $035014 x -$28 and $03501C x $168; charge $0385B4, shedding
$0385F0), `ToyChain_Spawn8B` $039896 (entry $034632, x $154; the B schedules $0380D8 / $03870C);
`ToyChain_MakeChild7C` $039A66 (7 links, table $039AE0, random speed $4C-$100, cruise $155) and
`ToyChain_MakeChild8B` $0399A6 (8 links on 7B's table $039AC4, speed $100, cruise $255). All
entries are screen-placed (y $E0xx) at x 330-360 or -20..-40: just outside the 4:3 view. Their
shedding / charge scripts carry no dc.l code pointer (scanned). Still raw, with no static reference
found: $036A06, $038B16, $038BF8, ring spawners $039088, $0390C2, $039102, $03913C, $039222, chain
bases $03967A, $039752 (and the makers they name, $0399C6 / $039A88).
**Raw code statically named (ws-tail's scan, 2026-10-30)**: every ROM long that points at a raw
spawner stub (`lea abs,a2` then `jmp` / `move.w #,d7`, or `jsr ToyChain_SpawnInline(pc)`) outside
every function span: 100 left after the seven above, none run by any corpus run at 4:3, each a fatal
"no C for code address" if an input reaches it (in either view; the gameplay mode's wider windows
reach wave steps sooner). Stage 1 (12): $0134DE, $0134F2, $01350A, $01393C, $013956, $013970,
$0139D8, $013A14, $013A32, $013A8C, $013B0E, $01B754 (the trigger tables at $012EFC-$012F7C,
$016F26). Stage 2 (27): $02570C, $029DF2 (12 refs, $023976...), $029EAA, $02A072, $02A0DC, $02A3FA,
$02A48A, $02A4F4, $02A584, $02A718, $02A7BC, $02A860, $02A904, $02B09E (10 refs), $02B28A, $02B48E
(10 refs), $02B7D6, $02B880, $02B9B6, $02BE86, $02C51C, $02C5A0, $02C624, $02CFF2, $02D408, $02E282,
$02F716 (the wave tables at $0220B0-$0234C2). Stage 3 (44): $03169E, $031760, $036416, $036628,
$036934, the makers $0399C6 / $039A88 / $039BC6 / $03A326 / $03A372 / $03A3BE / $03A3E4 / $03A430
/ $03AEBA / $03AF4A / $03AF78 / $03B064 / $03B092, $039C44, $03A952, $03AB1E, $03AB7C, $03ABE2,
$03ADBA, $03C636, and the 20 stubs $03FAF8-$03FDC2 (tables $03DD5C-$03DD8C, $03FB4C-$03FD3A). Stage
4 (17, 15 since corpus-p6 converted $043BA0 and $043C74): $0434DE, $043BA0, $043C74, $0443DC, $044418, $044460, $0444A8, $0453F4, $0455F2, $045850,
$0458C0, $04592C, $045BFC, $045D06, $045E44, $045F5A, $04601A (tables $041402-$0417B0,
$046AB6-$047136, $01317E/$0131EC). A referencing long can be a coincidence; the stub shape at the
target is not. Not converted: no corpus run reaches them; the playtest may (two players, other
routes, the gameplay mode).
**Stages 3-4: the raw code data names, converted (raw-stubs, 2026-10-30)**: the stage 3-4 half of the
list above (crane-crash took stages 1-2) plus a wider scan of banks 3-4. Every target is code by its
shape and is named by a table entry. Each got hand C on a sibling's contract and a synthetic case
(ruling 5, 2) at that sibling's first corpus call; 1212 -> 1317 functions. The scans (scratchpad
raw-stubs/: scan.py, scan2.py, scan3.py, unref.py, relref.py):
- raw-data longs masked to 24 bits, because a wave entry's spawner long carries an action byte on
  top. $036968's entries have $26, so ws-tail's scan missed them;
- the op operands that are code by statistics (Op2_SpawnChild +4, Op2/Op4/Op5/Op6_CallNative*);
- longs whose table neighbours are function starts;
- crane-crash's native-op scan (Op2_CallNativeRel, Op2/Op4_Native). It added the cat-head eyes and
  the stage 4 natives.
Converted:
- **Stage 4 spawners**: `Enemy_GasOrb_SpawnSideSweep` $0434DE, `Enemy_HoverPod_Spawn7G` $0443DC
  and the Freeze-fight pods `Enemy_HoverPod_Spawn4D` / `5E` / `6F` $044418 / $044460 / $0444A8 (new
  courses $44778-$447DE and attacks D-G, drop table $438DC), `Enemy_JetTrooper_SpawnHoverOnJitter`
  $0453F4 and `..._SpawnHoverOnFast` $0455F2, `Enemy_VentTank_SpawnFaster` / `Fastest` /
  `RandomSpeed` $045850 / $0458C0 / $04592C, `Enemy_GunTank_SpawnHeavy` / `ToughMid` / `Low` /
  `ToughFast` / `ToughSlow` $045BFC-$04601A. $0453F4's four ROM longs are a4-script words
  (`0004 53F4`, an Op4_Spawn/Op4_Fork operand and the next op), so no caller is known.
- **Stage 3 spawners**: `Enemy_PuffPlant_SpawnH` $036416, `Enemy_RabbitPot_SpawnE` $036628,
  `Enemy_DropCarrier_SpawnRabbitsB` / `C` $036934 / $036968, `Enemy_GiftBox_SpawnStay10C` $039C44,
  `ToyFigure_SpawnShed` $03A952, `Enemy_PropToy_SpawnChain4Right` / `TwinChain8` / `Chain5RightC` /
  `TwinChain6` $03AB1E-$03ADBA, `Enemy_ArenaDebris_SpawnBounceAtX` $03C636.
- **Chain makers**: `ToyChain_MakeBox10C` / `4B` / `4C` / `6` / `10D`, `ToyChain_MakeHanging5E` /
  `4` / `8B` / `6C` / `6D`, `ToyChain_MakeChild10B` / `5C`, `ToyChain_MakeDropIn12B`.
  `ToyChain_MakeBox4B` / `4C` / `6` are three of the four random picks of
  Enemy_GiftBox_SpawnRoamRandom, a box the corpus runs; only the pick that is ToyChain_MakeBox5 was
  code. The four makers 10B, 5C, DropIn12B and Box10D are named only by the scripts of the
  unreferenced raw stubs $03967A, $039752, $039B50 and $03A264.
- **Missiles**: `Enemy_Missile_SpawnAgileSlow` $03169E and `Enemy_Missile_SpawnTurning` $031760.
  Nothing names their launch subroutines $031690 / $031752 (no Gosub, GosubArgs or long).
- **The Hatter road schedule** (the "Hatter road" section's never-run commands): the schedules
  the boss's scripts start (StartCoroutine2Rel targets $03F58C-$03FAB0) use set loop, random and
  spawn kinds 1-7, so these could stop a later fight phase.
  - The handlers: `Boss_Hatter_RoadCmdWait` / `Gosub` / `Return` / `SetLoop` / `Loop` / `Random`
    $03DCDC-$03DD1E.
  - The 13 raw `Boss_Hatter_RoadCmdTbl_*` entries and the picks `Boss_Hatter_RoadCmdSpawn1-7`
    $03DD5A-$03DD8A.
  - The kinds `Boss_Hatter_SpawnRoadSplitRight` / `SplitLeft` / `DriftRight` / `DriftLeft` /
    `SplitRightSoon` / `SplitLeftSoon` / `HazardNear` (kinds 1-7, $3FB24 ...). The splitters
    burst into three pieces each, `Boss_Hatter_SpawnRoadPiece{Right,Left,RightSlow,LeftSlow}{1,2,3}`.
  - `Stage3_RoadPlaceNear` $03DDE6 (distance $840): it was labelled Scr2_03DDE6.
  - The pieces spawn through three raw bank-0 routines, now code: `Obj_SpawnEnemyAfter` $008E22,
    `Obj_SpawnAfter` $008F3C and `Obj_NewMultiAfter` $00899A. They are Obj_SpawnEnemy /
    Obj_SpawnUnlinked / Obj_NewMulti with the new object linked right after the running one. Their
    unreferenced twins $008E3C, $008EC4, $008EDE and $008F36 stay raw.
  - HatterRoadScene.loop is now the byte the code uses.
- **Natives**:
  - The cat-head's eye attacks: `Boss_CatHead_EyeSparkLeft` / `Right`, `Boss_CatHead_EyeShotLeft` /
    `Right`, their shared `Boss_CatHead_EyeShotAt` / `Boss_CatHead_EyeFxAt` and the helper
    `Boss_CatHead_SwayPoint` $03B760-$03B7F1, called by Op2_CallNativeRel in the attack coroutines
    $03C43C-$03C47E. They take a point through the arena's sway shear, and CatHeadArena gains unk_16
    for the long the helper reads at +$14.
  - `Enemy_JetTrooper_RideTubeScroll` $042420 and `Fx_RideTubeScroll` $04291A: x += the tunnel
    speed.
  - `Enemy_JetTrooper_SteerEighthTurn` $042484.
  - `Boss_Freeze_ShotRecoilLeft` / `Down` / `Up` $0467C0 / $04689E / $0468F4.
  - `Continue_DrawNoCreditsText` $049236 (Tbl_049480 'NO CREDITS REMAINING': the continue screen
    with no credits left).
  - `Player_CopyPadToVar2Robin` $04FE7E (Robin's twin of Player_CopyPadToVar2).
Left raw on purpose: about 25 stub-shaped spans that nothing names. Among them are $031068,
$0311C6, $03129A, $03135C, $0364E0, $0365D6, $0365FC, $0366C0, $036A06, $038B16, $038BF8, the chain
bases $03967A / $039752 / $039B50, $039DDA, $039EF6, $03A0C6, $03A264, the makers $0399E6 /
$039A26 / $039A46 / $03A47C, and $045776, $0459AC, $045D38, $045F90. No ROM long, table or op operand
names them (unref.py lists them). Each would still be a fatal stop if some computed path the scans
miss reached it.
**Toy-chain makers, gift boxes and propeller toys** ($0398FE-$03A91B). The chain children
(`ToyChain_Make*`, all `jsr ToyChain_SpawnInline` + 10 inline bytes) are started by their holder
with Op2_SpawnChild into field $2A. Table entries are 28 bytes (+0 var 0, +$A/+$C growth, +$E
energy, +$10/+$12 link damage/knockback, +$14 hit callback EnemyAtk_HitKnockSpeed). Makers:
`ToyChain_MakeChild{7,8,5B,6B,7B,10}` for the ToyChain_Spawn* bases (body Scr2_0379DA: the
cruise x is the sway speed); **drop-in chains** `ToyChain_SpawnDropIn{12,9}` (an invisible base,
body Scr2_037B12, falls from above the screen, dust on landing; f92290) + `ToyChain_MakeDropIn
{12,9}` (body $037B8A). **Gift boxes** (`Enemy_GiftBox_Spawn*`, body Scr2_037C02, frames $45D8
wrapped present, $45E2-$461E bursting): $3C0 ticks after the spawn the box opens and a chain grows
out of it (`ToyChain_MakeBox{1,4,5,8,10A,10B,10Wide}`, body $037DD6: the sway is an oscillator),
then the box hops (Op2_JumpAnim) in place (Stay) or roaming at cruise x $2AA (Roam).
**Propeller toys** (`Enemy_PropToy_Spawn*`, body $037E20/$037E26: a fat toy with a propeller cap,
frames $4484 + $448E/$4498, energy $40, score $32; sections $3200 forest, $3300 tunnel, $3400):
`SpawnFlyBy` flies across alone (placed with item/health spawn actions); the others carry a
hanging chain (`ToyChain_MakeHanging6/10/10B`, body $03802E, charge cycle $0380A8) that sheds
figures through ToyChain_RemoveLastLink + Spawn_AtAnchor ($03A91C, or ToyFigure_SpawnStopHop).
Waypoints use the 2-argument entry $2D08 of Op2_GosubArgs (Arg1 = y, Arg0 = x) into
Op2_AimAtArgPoint ($037EC2). Never run, still raw: chain makers $0399A6, $0399C6, $0399E6,
$039A26, $039A46, $039A66, $039A88, $039BC6, $03A326, $03A372, $03A3BE, $03A3E4, $03A430 (three
of the four picks of the random subroutine $03A11C; the TAS drew $03A40A), $03A47C; drop-in base
$039B50; gift boxes $039C44 (Scr2_039C50), $039DDA, $039EF6, $03A0C6, $03A264.
**More propeller toys with chains** ($03A91C-$03B0EE; forest TAS f93540-f97692, shots f93825/f97400:
the carrier at screen (185,32) in RAM f97380 is the propeller toy, the chain hangs below it):
`Enemy_PropToy_SpawnChain{8,6,5}{Left,Right}` (+ `...5LeftB/5RightB`) enter from the left (x -$28)
or right (x $168, H-flipped), `...SpawnTwinChain10` carries two chains (fields $2A/$2C). Chain
makers `ToyChain_MakeHanging{8,6B,5A-5D,10C,10D}` (tables $03B174 link energy $60, $03B1AC energy
1, $03B1E4/$03B200 energy $20). The wave entries' top byte gives the toy a drop ($29 health, $1C
extra life for the twin). Shed figure `ToyFigure_SpawnWithDrop` ($03A91C: random drop
$29/$2B/$2D/$2E via the gosub $03B11C). Never run, still raw inside script spans: propeller-toy
spawner $03ADBA (Scr2_03ADC6, two 6-link chains $03B064/$03B092) and chain makers $03AEBA (5
links), $03AF4A (4), $03AF78 (8), $03B064, $03B092 (6).
**Cat-head boss** ($03B260-$03BDC5, scene $03FFBE whose trigger table Tbl_033F20 holds only
`Boss_CatHead_Spawn`; TAS f87285-f90392, end of the $3100 backlot): a giant grinning cat face on
plane A that follows the head object (`Boss_CatHead_PublishPos` -> g_SceneVars+8/+$A ->
`Boss_CatHead_FrameScroll`, the scene's per-frame hook, which also calls the raster routine in
g_SceneVars+0). Children: the paws (`..._SpawnLeftPaw/RightPaw`, claw swipes) and an invisible
**raster driver** (`..._SpawnRasterDriver`) whose position the `Driver*` natives copy into
g_SceneVars for three rasters, each started by a `Start*Raster` that sets scroll mode 4 with a
one-shot g_ScrollVIntFn (`*VInt`: sets the real mode $2C/$3C and VDP reg $0B in the V-blank):
**curtain** (plane B's red stage curtain squeezed into the top g_SceneVars+4 lines, H-int line
V-scroll; it rises at the start and closes/reopens after the defeat, f87400/f89640), **sway**
(plane A line H-scroll + column V-scroll ramps = a small rotation by +$10 about the pivot
+$12/+$14, plus a bulge +$C; `..._DriverSettle` damps it), **fall** (the face plate tips flat onto
the floor line $AA, g_SceneVars+6; a dark mechanism behind, f90150). `Boss_CatHead_TakeDamage`:
shots (striker id not 1/2) do a quarter damage; energy $1600/$2000. **Arena debris**
(`Enemy_ArenaDebris_Spawn{Bounce,Thud}`, wave Scr5_03C4A8, body $03B5EC): tumbling objects in
three random looks fall from y -$14, bounce and puff smoke; `..._HitPlayer` knocks the player back
and makes the debris bounce. Never run, still raw: debris spawner $03C636 (Scr2_03C642) inside
the Scr2_03C62A span; attack scripts Scr2_03BE2A, Scr2_03C066, Scr2_03C1C6, Scr2_03C34C.
**Forest-clearing reveal and robot boss** ($03C64E-$03CFFF; clearing scene loader Scr6_040054,
TAS f97887-f101674). The loader calls `Stage3_SpawnRevealDirector` (Op6_CallNativeRetry); the
director (Scr2_03CB90, boss music $A) cycles palette line 3 every frame (`Stage3_InitLine3Cycle`
/ `Stage3_StepLine3Cycle`: a walker through RGB space in g_SceneVars2+$22..+$2D, 15 consecutive
steps fill colours 1-15) and assembles plane B's picture cell by cell: `Stage3_RevealInit` (cell
map = 40x22 words in g_SceneWorkBuf, cursor g_SceneVars+0/+2/+4, seed +6, V-int object +8),
36 `Stage3_RevealBurst`s (a spot of a 9x4 grid in LCG order, a spinning `ToyFigure_SpawnFlyer`
thrown from it, a random-walk patch of cells), then `Stage3_RevealNextCell` fills the rest with
flashes and shakes, and `Boss_ForestRobot_Spawn` drops in the boss (a giant copper robot,
f101300). Each cell = the a4 script Scr2_03CE3E (labelled a2, but it is a4: Op4_Spawn,
Op4_CallNative): `Stage3_RevealPlaceTile` queues the cell's name-table word (g_SceneVars+$A count,
+$C entries of VDP command long + word; flushed by the V-int object `Stage3_RevealVIntFlush`) and
throws a one-tile sprite drawn straight into the SAT by `Stage3_RevealDrawTile`.
**Stage 3 section routines** ($03FE94-$040629; Tbl_00A0AC): $3100 `Stage3_RunBacklot` (f79329),
$3101 `Stage3_RunCatHeadBoss` (f87254), $3200 `Stage3_RunMushroomForest` (f90392), $3300
`Stage3_RunSpottedRoom` (f101674), $3400 `Stage3_RunCardHall` (f108293), $3401
`Stage3_RunHatterRoad` (f113271); the backlot, forest and spotted room refill both players'
health. **Spotted room** (a red room of black holes in perspective, png f102000): scroll mode $2C;
`Stage3_SpottedRoomScroll` writes 224 plane-A line H-scrolls (88 ceiling / 48 back wall / 88
floor lines spreading from g_Stage3RoomSlide) and 20 column V-scrolls, all sheared by an undamped
wobble spring g_Stage3RoomSway. **Card hall**: `Stage3_CardHallFloorScroll` = perspective line
scroll of the checkerboard floor (80 lines from line 104 - view y) and 40 foreground lines bent
by g_Shake2Pos.
**Hatter road** (section $3401, $03D000-$040C85; TAS f113271-f116145, png f113400-f115800). The
checkered road through space is plane A drawn with line scroll (mode $28 + H-int V-scroll): the
road driver object (`Stage3_SpawnRoadDriver`, Scr2_04061A) runs `Stage3_RoadCameraFollow` and
`Stage3_DrawRoad` every frame; up to 18 checker rows are written from the bottom line up through
the unrolled `Stage3_DrawRoadLinesRun` (5 more never-run units at $040944-$04097F) and each row's
geometry goes to g_SceneVars+$12.. (8 bytes: line, line, x offset, scale) for the road objects.
Road state in g_SceneVars2: +8 speed (16.16; +$C its integer), +$10 distance (+$E mod $80 = checker
phase), +$12 sideways scroll (-camera x * 9/8), +$16 bend / +$1C pitch (16.16, = the steerer's
world x/y / 16), +$1A horizon line ($64 + shake), +$3A/+$3C screen origin of road objects, +$3E
spin index of thrown toys (+4 mod $F0). The boss's invisible child **road steerer**
(`Boss_Hatter_SpawnRoadSteer`, Scr2_03DA8C, field $12) moves (seek/oscillator ops) and publishes
its position as bend/pitch (`Boss_Hatter_SteerRoad`); `Boss_Hatter_SetRoadSpeed` (2 words) sets
the eased road speed (0 at the end, then Section_StartExitN). **Road schedule**: the boss steps a
byte-pair command list held in its coroutine slot 2 (`Boss_Hatter_StepRoadSchedule`, waits
counted in checker rows; commands in its header; table `Boss_Hatter_RoadCmdTbl`): kind 0 spawns
`Boss_Hatter_SpawnRoadHazard` (a perspective-scaled object, RAM frames $FC40-$FDA8 by distance,
frame words $03DF14-$03E033; attack box only at the players' depth; a hit pushes it back a row);
kinds 1-7 ($3FB24, $3FBC2, $3FD9C, $3FDC2, $3FC60, $3FCFE, $3FAF8) never ran. A pink swirl marks
the road's far end (`Stage3_SpawnRoadEndSwirl`, unlinked, run after the objects by
`Stage3_RunRoadEndSwirl`). The road schedule's commands 1 and 3-7, the spawn kinds 1-7 and the
placement variant at $03DDE6 (distance $840) never ran, but the schedules use them. They were
raw until raw-stubs (2026-10-30) converted them, now `Boss_Hatter_RoadCmdWait` ...
`Boss_Hatter_SpawnRoad*` and `Stage3_RoadPlaceNear`: the paragraph "Stages 3-4: the raw code data
names" in the toy section.
**Stage 4 enemies** ($041EC2-$046169; sections (Tbl_00A0AC) $4000 hall $475C2 f116171, $4300
lab $47772 f119385, $4400 jetpack tube $479EE f122651, $4401 Mr. Freeze fight $47BD0 f128749;
trigger tables Tbl_040F4A / Tbl_041000 / Tbl_041124 / Tbl_041138, wave scripts in the spans
Scr5_0413AC and Scr2_0417CC, which are a5 wave scripts + spawn entries). Bodies live in the spans
Scr2_04204C-Scr2_0430C2. **Gas orb** (`Enemy_GasOrb_*`, body $041E4A, energy $1C0: a purple orb
with a red eye, tube section and the Freeze fight): up to three gas jets 120 degrees apart
(`Enemy_GasOrb_RunJets` / `StepJet`, script var 3 bits 1/2/4, puff blocks in a6 +$12/$14/$16; 5
puffs per jet, frames $497A-$49CA with a hit flag table at Scr2_04204C). The jets have their own
motion block at $E(a6) (Op2_LoadA3Handle $E before coroutines 3/1 and the jet natives): heading
+$C, puff start speed = its cruise y $34 and accel = $28, which the attack scripts set ($A00 in
$440FC-$443D6, $1400 in $43E1C-$440FA). The attack (vector 1, run as coroutine 1 on that block)
sets Op2_OrVar3/AndVar3 jet masks and Op2_CruiseLeft/Right on the block's x, which
Enemy_GasOrb_SpinJets turns into jet rotation. Spawners differ in the attack pair: Pair = two-jet / single-jet
sweeps, Triple = jet sequence / all three, Far = the $1400 set; `...Boss*` are spawned by the
Freeze fight scripts (Scr2_046D32/046F7C). Helpers: `Stage4_TreadFrameByX` ((x/2) mod 3 tread
frame, vent tank and gun tank), `Enemy_BombBot_FrameByCameraX`, the shot/puff hit callbacks
`Enemy_JetTrooper_ShotHit` (HitTouch only) and `Enemy_VentTank_PuffHit` (blows the player up:
stun 2, y speed -4), `Enemy_GunTank_ClampAimUp` (turret aims only into the upper half).
**Hover pods** (hall $4000, `Enemy_HoverPod_Spawn{1,2,3}{A,B,C}`, body $042152, energy $1C0: a
purple striped sphere with a thruster jet, frames by heading $48F8-$4970, oscillator hover): wave
entries at screen x $A0 start them above the screen (JitterPos + op $1640 -$96); digit = course
(coroutine 0 $44836 / $4487A / $44892, random screen-area waypoints $452xx-$4539A), letter =
attack (coroutine 1 $44D68 rounds of 6 shots / $44E14 bursts of 7 / $44E82 8 spaced shots).
**Jetpack troopers** (tube $4400, `Enemy_JetTrooper_Spawn*`, body $0423CE, energy $1B0/$1E0:
white armoured men with a jetpack and a gun, f125700): they enter at x $168 or -$28 and hold
player-relative windows (Op2_GosubArgs -> Op2_SetPlayerWindowFromArgs $424B0; words y far, y
near, x far, x near): HoverOn, HoverLeft, HoverRight, SideToSide, Above, Below; shots = a4
$42964 (callback Enemy_JetTrooper_ShotHit). The body's native fragment $04241E adds
g_SceneVars+4 to x (the tube's scroll drift, inferred).
**Mad Hatter boss** (`Boss_Hatter_Spawn`, Scr2_03D032; energy $1D00/$2D00, score 5000): a giant
upside-down top hat with the Hatter standing on its brim, flying above the road. Hat animation
Scr2_03D79E picks poses by `Boss_Hatter_SwitchCruiseDir`. Attacks (coroutine/vector scripts
Scr2_03E034, Scr2_03E39A; Scr2_03E532/03EF80 never ran): bomb rabbits dropped from the hat
(`Boss_Hatter_DropHopRabbit` y+10, `..._DropRabbit` y+70, the stage 3 Enemy_BombRabbit spawners),
toy figures thrown in arcs (`Boss_Hatter_LaunchFx` runs $03F494/$03F548 = Op4_Spawn + Arc_PlanToArgs;
the toy a4 script $03D9F0 spins by g_SceneVars2+$3E and blinks outlined, `Boss_Hatter_ThrownToyFrame`),
the road hazards of the road schedule, and a **little top hat** (RAM frame $FDD0) that pops out of
the hat and falls back in (`Boss_Hatter_PopLittleHat` + `LittleHatSetOwner`/`LittleHatFollow`; the
owner a6 is passed through g_ArcTargetX). Defeat (f115594-): the little hat jumps out as an object
(`Boss_Hatter_SpawnLittleHat`, energy $120; shot down it drops an extra life,
`Boss_Hatter_LittleHatDropLife`, f115744), the hat drops and bounces (`Boss_Hatter_StartBounce` /
`StepBounce`, drawn offset 24 -> 64 px, 3/4 rebound), the steerer stops the road and calls
Section_StartExitN. Gas orb: `Enemy_GasOrb_SpinJets` ($041E9A) turns the jets' heading (motion
block $E(a6) +$C) by twice that block's x move each frame.
**Lab and hall ground enemies** (floor heights by Op2_PlaceYOnScreen): **lab techs**
(`Enemy_LabTech_Spawn`, body $042A34, energy $50: goggled men in white coats, f119460);
**vent tanks** (`Enemy_VentTank_Spawn{Slow,Mid,Fast}`, body $042D74, energy $300: a tracked
machine with five nozzles, f121230) drive at the player, stop and vent gas puffs upward (a4
$42F4E via Fx_SpawnAtOfs (0, -$18) and the fork scripts $45B8C-$45BE0); **gun tanks**
(`Enemy_GunTank_Spawn{,Tough}`, body $042FEC, energy $100/$200: a domed tracked tank whose
twin-gun turret is a second sprite block at $10(a6), f119760) fire bursts at the player;
**bomb bots** (`Enemy_BombBot_Spawn{,Hall}` + $04616A, body $0432E2, energy $100: a silver
capsule robot on a wheel, f117480) roll after the player and, when the random timer 0 fuse
runs out, stop, flash (anim $433B4, sound $44) and blow up (a4 $433DA) without scoring.
Converted by corpus-p6 (2026-10-30): `Enemy_GasOrb_SpawnBossPairJetsFar` $043C74 (Scr2_043C80,
coroutine 1 $43D18: $43E1C / $43E66; scenario-sec-4401-freeze runs it on the reference) and its
static sibling `Enemy_GasOrb_SpawnBossTripleJets` $043BA0 (Scr2_043BAC, coroutine 1 $43C44:
$4425E / $441CE; the Freeze scripts' longs $046FF4 / $04701E): the four boss spawners' scripts are
equal but for coroutine 1. Never run, still raw (spawner stubs inside the script spans): gas orb
$0434DE; hover pods $0443DC, $044418, $044460, $0444A8; jetpack troopers $0453F4, $0455F2
(in the tube wave's entries $41CD4/$41CDC, never picked), $045776; vent tanks $045850, $0458C0,
$04592C, $0459AC; gun tanks $045BFC, $045D06, $045D38, $045E44, $045F5A, $045F90, $04601A.

**Stage 4 sections and scroll** ($0471C0-$047CCE). `Stage4_RunHall` ($4000, refills both
players' health; `Stage4_HallFloorRipple`: plane B lines 160-223 = sine ripple), `Stage4_RunLab`
($4300; `Stage4_LabScroll` -> `LabFloorLines` / `LabUpperLines` (plane A perspective, floor x =
view x * 35/32 in 87-px segments) / `LabPlaneBLines`), `Stage4_RunTube` ($4400, mode $10
interleaved line pairs; `Stage4_TubeScroll`: ring position wraps at 64 px, speed g_SceneVars+4
eases to g_SceneVars), `Stage4_RunFreezeFight` ($4401). **Translucent cones** (hall and Freeze
fight): three slots g_ConeSlots ($1E bytes each), owned by the objects whose scripts call (pointer
in $1E(a6)): the calls at $0421EC-$04226C are in the hover pods' body $042152, Scr2_0451B6 in the
hover pods' scripts (corpus-p6, 2026-10-30: in the TAS a slot is in use on 948 frames of the hall
$4000, on 0 of the tube $4400 (15 gas orbs) and of the whole Freeze fight $4401, f128749-130772;
0 also in the 13 Freeze fights played from scenario-level-skip's states: the Freeze fight sets the
raster up and draws no cone), through a2 natives `Stage4_ConeAlloc` (switch: 0 ok / 1 full), `...SetInline` (byte
colour 8/$10/$18, byte mode 0 spin / 1 sweep once / 2 sweep / 3 close, word width, word length),
`...Update` (every frame: apex = owner, width/length ease), `...Free`. `Stage4_DrawCones` draws
ONE slot per frame (round robin g_ConeDrawSlot) as a span triangle (mode $38, set by
`Stage4_InitConeRaster`): two far-edge points rotated by two angle sets through g_Rot3DOuter
and projected; the slot's colour set (Tbl_047508: red/green/blue, 4 words) goes into palette
line 3 colours 10/11/12/14. **Mr. Freeze** (`Boss_Freeze_Spawn` = Tbl_041138's trigger,
f128791): Scr2_046D32 = coroutine 0 (moves, spawns the boss gas orbs) + Goto the body $04619C in
Scr2_046176 (energy $3000/$4000, meter): he rides a hover platform whose frame follows the damage
stage var 0 (`Boss_Freeze_GetDamageStage`, +1 at 96..70 %); at 15 % the platform blows up and
he drops $50 px to the floor (`Boss_Freeze_DropStep`, f130401-130448) and fights on foot.
`Boss_Freeze_ShotRecoilRight` = the firing animation's native (impulse +3; the -3 twin at
$0467BE is raw, never run). $046176-$04619B is the bomb bot spawner `Enemy_BombBot_SpawnHallNoDrop`'s
prologue (body $0432E2), not Freeze. **Scene loops** shared by story/attract/ending/continue:
`Scene_LoopSkippable` (Start skips after g_SceneSkipDelay unless g_IntroNoSkip),
`...Delayed` (delay $1E), `...KeepMode`, `Scene_LoopUnskippable`.

**Story, attract, ending and continue scenes** ($0484F6-$0491D1). Each scene routine = plain
init + a6 art script + one "camera" object (`Story_Spawn*Camera` etc., `lea Scr2_x / jmp
Obj_SpawnEnemy`) whose native turns its y (or x) into the plane scroll, then a scene loop; the
object's script ends the scene (Scene_EndAfterDelay). Scene-list entries (Tbl_00A136/Tbl_00A152)
point at interior labels, after the previous scene's native. Idle story (title timeout,
Tbl_00A136; idle run cycle 3925 frames from f1722): `Story_AsylumGate` (gate + moon, also the
first ending scene as `Ending_AsylumGate`), `Story_Monitors` (four security monitors, then the
ALARM: red tint toggles, `Story_MonitorsAlarmScroll`), `Story_Searchlight` (lamp before the moon,
sine-wobbled plane A lines), `Story_GuardTower`, `Story_AsylumFacade` (lightning flashes),
then $049E06. Attract (Tbl_00A152): $049DA6, $049F62, `Attract_Skyscraper` (camera climbs a
tower, line-tint lightning: line 3 up, lines 0-2 down), `Attract_Rooftop` (Batman and Robin on a
skylight), then the title. `Continue_Run` = the continue screen (Freeze's head, countdown
monitors); `Continue_BranchHasCredits` picks the script tail: no credits -> $0491C6, never run.

**Front-end scenes** ($491D2-$04A1C7). Each is a scene routine (`X_Run`: Scene_InitPlain + VM6_Run +
spawners, then one of the Scene_Loop* above) whose objects end it with Scene_EndAfterDelay. Attract
(Tbl_00A152): `SegaLogo_Run` (colours 8-15 of line 0 rotate every 3 frames), `Copyright_Run` (DC /
SEGA notice text map), ...; the story list ends with `ClockworkLogo_Run` (developer logo, line-3
colour cycle). `Title_Run`: 500 idle frames -> story; a piecewise palette fade-in
(`Title_StartPaletteFade`, 7 channels in Tbl_04984C lighting colour ranges one after another via
Pal_CopyBright; Start skips it); Start opens START / OPTIONS (`Title_MenuStep`, picture dimmed by
`Title_DimPicture`); `Options_Step` edits g_StartCredits / g_OptMusicTest / g_OptSoundTest (A/B/C
plays the test). `PlayerSelect_Run`: plane A line scroll split in two halves (top Batman = the
chooser's x, bottom Robin = its y word), each slid in/out by `PlayerSelect_Slide*Half`; A/B/C/Up/Down
toggle g_P1IsRobin, Start or 300 frames confirm. `BatmobileIntro_Run` opens every stage
(Stage_Intro1-4). Continue screen (Continue_Run, Mr. Freeze's head between two monitors): credits left ->
a 60-frame-per-step countdown on the monitors (a6 scripts $48F8C-$490FC) and the Start check
(`Continue_UseCredit` takes a credit, result 1); out of time -> shakes, flashes, slow white fade.
No credits ($491C6, never run): a 20-frame countdown with NO CREDITS REMAINING. `GameOver_Run`:
GAME OVER sprite text over a scrolling snowy skyline (camera +2 px a frame; the text adds 2 px to
its own move to keep pace), 1000 frames. Mislabelled data: Scr4_0494EA and Tbl_049F94 are a2 scripts.

**Stage cards and the last ending scenes** ($04A1C8-$04A74A). Each `Stage_IntroN` list = the
shared Batmobile opening `BatmobileIntro_Run` (its object calls `BatmobileIntro_Animate` every frame:
palette line 0 colours 2-13 rotate in 3-colour groups every 4 frames, plane A jumps 320 px right /
1 px down on 2 of 6 frames) then `StageCard_StageN`: a director (`StageCard_SpawnDirectorN`:
Scene_EndAfterDelay $C8, palette line 3 except stage 3, then two CallNativeRelSaved 30 frames
apart) spawns `StageCard_SpawnTitleN` (LEVEL ONE.. from the left) and `..SubtitleN` (from the
right); the strings are at $14092-$140FB (HAPPY BIRTHDAY TO ME!, A TWO SIDED STORY, TEA TIME!,
SNOW IN JULY?). `Main_Ending` after `Ending_AsylumGate`: `Ending_FreezeCell` (narrator
`Ending_SpawnCellNarrator`, boss class, whose coroutine in Tbl_04A74A types five text pages
'IN A FREEZING CELL IN ARKHAM ASYLUM...' with fades, the Freeze picture and
`Ending_SpawnSnowGlobe`), then `Ending_Epilogue` (Batman's face, 'REVENGE IS A DISH BEST SERVED
CHILLED...' and the Joker's laughter; text at $4ABDE). Both narrators' loops call a native
(`Ending_CellTextStep` / `Ending_EpilogueTextStep`: TextMap_Run while typing, TextMap_Upload).
Tbl_04A74A is mostly that a2 coroutine script plus TextMap text, not a plain table.
`Player_CopyPadToVar2` ($04D342): Batman's jetpack-tunnel script latches var 0 into var 2 on a
pad release (var 2 picks the pose).

## Data formats

- **Data labels** are planned by `uv run tools/datamap.py` from the corpus and formats, then
  applied with `dkit disasm label --batch`. Kinds:
  - `Lz_`: an LZ block;
  - `Art_`: DMA'd to VRAM, or a frame-table art block;
  - `Map_`: a sprite piece list;
  - `ScrN_`: an aN-VM script, read by that VM's handlers, or recognized by its handler words;
  - `Tbl_`: read by the code named in the comment;
  - `Snd_`, `Fill_`.
  Every comment says what the label rests on; "inferred" marks anything not seen at run time.
- **Sprite frame table** `Tbl_1DDB60`-$1E27F0 has 1,960 10-byte entries: word art handle, long
  piece list, long art block.
  - **Art block** (in $100000-$1CFFFF): word DMA cost, word chunk count n, n chunk codes (code/4
    = tiles in the chunk), then the tiles at 32 bytes each. The extents chain block to block.
  - **Piece list** ($1D0F96-$1DDB60): word pieces-1, then 12-byte pieces. All 1,958 gaps between
    lists are explained by this format.
  - 304 of the 1,565 art blocks and 456 of the 1,959 piece lists are never displayed in the
    corpus. They are labelled from the format with "never displayed in the corpus".

- Compressed blocks: 103, $05A022-$0FFD3E, 674,239 bytes packed / 1,497,804 unpacked; 91 read by
  the corpus. Never read (scenes no run reaches; pointed to only from unread scripts at
  $048F06-$04A594): $0CCD66, $0D1200, $0D8244, $0DFBE6, $0E0DFA, $0E466C, $0E6858, $0F9352,
  $0FA352, $0FE6BE, $0FF9E8, $0FFB54 (no pointer found).
- Music banks $1E8000-$200000: **Z80 bank reads are invisible to coverage** (the core never sets
  DataZ80), so they always show never-read: label them statically from the module format.

## Game flow

Title (START / OPTIONS: credits, music test, sound test) → player select (Batman / Robin) →
Batmobile intro → stage card. Four stages: 1 Joker (Harley mid-boss), 2 Two-Face (then the long
shoot-'em-up section), 3 Mad Hatter, 4 Mr. Freeze. P2 joins in play with Start (as Robin; port 2
plugged). Lives out → continue countdown → GAME OVER → SEGA logo. Pause + B A ↓ B A ↓ ← ↑ C skips
the current section. Ending: Mr. Freeze text, then the attract loop. Attract: intro, title, the
Arkham monitors story.

## Explained code-as-data (R5)

List each `code_as_data` span start (`$XXXXXX`) with its explanation.

- `$00B232`: the head of an unrolled loop ($00B232-$00B932, 8-byte units entered by
  `jmp $B932(pc,d7.w)` at $00B938) that sits right after the H-int handler source $B20C-$B231.
  `$00B1B6` copies $4C bytes from $B20C, so its copy loop at $00B1C4 also reads $B232-$B257 as
  data; those bytes land at $FFE586+ and never run in RAM. In ROM they are real code.
  Evidence: kickoff RAM-code measurement (run x-agent2-hint, `--trace-writes`); the
  computed-entry run in dkit.toml `[recomp]`.

- `$0106D8` and `$0106FA`: 8-byte reads by `move.l $18(a3),$3e(a6)` / `move.l $1c(a3),$42(a6)`
  at $009204 in `Fx_RunScript`, only in fuzz runs (fuzz-68400-s2, fuzz-25200-s1, fuzz-106200-s2,
  which also read $010702). `Fx_RunScript` copies two longs from a3 into a spawned object. On this
  path (`Hitbox_SpawnAtContact`, entered from the handlers at $0106D4/$0106FC), nothing sets a3, so it holds
  whatever the caller left. Under fuzzed input that pointed into this handler code. The values
  read are code bytes, not data (inferred from the code path; a3's value was not traced).
- `$040C78`: one 0x240-byte read ($040C78-$040EB8) by `move.w -$20(a2),d0` at $0407FA, only in
  fuzz-113400-s1. The bytes after the `jmp $40c44(pc,d7.w)` at $040C82 are a table; this read
  starts $20 bytes before it, in the tail of the unrolled block code ($040C78-$040C85). Inferred:
  an index out of range under random input.
- `$042FBE`: the last 4 bytes of the hit callback `Enemy_VentTank_PuffHit` (the `$001C` displacement of
  `move.w #$FFFC,$1C(a1)` and its `rts`). The TAS reads them as part of the record
  $042FBE-$043072, whose readers are the VMs' dispatch ops: `movea.w (a4)+,a0` at $0067FC (after
  `movea.l $c(a6),a4`), `movea.w (a2)+,a0` at $00382E, and others. The script that follows at
  $042FC2 (raw) is read from there; which op reads the 4 code bytes isn't resolved (open question).

## Unknown (S5)

Each remaining unlabelled never-read span (`$XXXXXX`), with what is known.

Generated by `uv run tools/datamap.py --unknown FILE` (2026-10-27): 318 spans, 37872 bytes (1.81% of the ROM). None is read or DMA'd by any of the 153 corpus runs, none is addressed by a code operand, and no format rule (LZ block, frame-table art or piece list, VM script words, zero fill, known sound/fill region) identifies it. Most are short gaps between labelled objects in the code/script region (unrun script tails, padding).

- `$0007EA`-$0007FC (18 bytes): never read
- `$000ABE`-$000ACC (14 bytes): never read
- `$000B52`-$000B5E (12 bytes): never read
- `$000BA6`-$000BD0 (42 bytes): never read
- `$000C20`-$000C2E (14 bytes): never read
- `$000C46`-$000C5E (24 bytes): never read
- `$000C76`-$000C8E (24 bytes): never read
- `$000CF6`-$000D26 (48 bytes): never read
- `$000D34`-$000D42 (14 bytes): never read
- `$000D5E`-$000DE8 (138 bytes): never read
- `$000DF6`-$000E3E (72 bytes): never read
- `$000F0E`-$000F2E (32 bytes): never read
- `$000F44`-$000F80 (60 bytes): never read
- `$000F88`-$000F9A (18 bytes): never read
- `$000FB8`-$001000 (72 bytes): never read
- `$00101E`-$00105A (60 bytes): never read
- `$001092`-$0010E2 (80 bytes): never read
- `$0010F8`-$001134 (60 bytes): never read
- `$00114E`-$001156 (8 bytes): never read
- `$00116C`-$0011B4 (72 bytes): never read
- `$0011D2`-$00120E (60 bytes): never read
- `$001236`-$001240 (10 bytes): never read
- `$00126E`-$001282 (20 bytes): never read
- `$0012D6`-$0012E4 (14 bytes): never read
- `$00134A`-$001364 (26 bytes): never read
- `$001406`-$001450 (74 bytes): never read
- `$00149C`-$0014F0 (84 bytes): never read
- `$0016EE`-$001708 (26 bytes): never read
- `$00172C`-$001736 (10 bytes): never read
- `$0017E2`-$0017EA (8 bytes): never read
- `$001846`-$00185A (20 bytes): never read
- `$001872`-$001884 (18 bytes): never read
- `$001920`-$001922 (2 bytes): never read
- `$00194C`-$00194E (2 bytes): never read
- `$0019C0`-$0019EC (44 bytes): never read
- `$0019F0`-$0019F2 (2 bytes): never read
- `$001A36`-$001A5C (38 bytes): never read
- `$001A60`-$001A62 (2 bytes): never read
- `$001A8C`-$001A8E (2 bytes): never read
- `$001B00`-$001B2C (44 bytes): never read
- `$001B30`-$001B32 (2 bytes): never read
- `$001B76`-$001B98 (34 bytes): never read
- `$001BCC`-$001BDA (14 bytes): never read
- `$001BE2`-$001BEC (10 bytes): never read
- `$001C10`-$001C3C (44 bytes): never read
- `$001E30`-$001E66 (54 bytes): never read
- `$001E9E`-$001EAC (14 bytes): never read
- `$001EBC`-$001EEA (46 bytes): never read
- `$001F08`-$001F38 (48 bytes): never read
- `$001F46`-$001F66 (32 bytes): never read
- `$001F84`-$001FB4 (48 bytes): never read
- `$001FF8`-$002002 (10 bytes): never read
- `$002012`-$002020 (14 bytes): never read
- `$00205E`-$00208E (48 bytes): never read
- `$0020DC`-$0020F2 (22 bytes): never read
- `$002130`-$002150 (32 bytes): never read
- `$002160`-$002180 (32 bytes): never read
- `$002192`-$0021D2 (64 bytes): never read
- `$0021E2`-$002232 (80 bytes): never read
- `$00224A`-$002252 (8 bytes): never read
- `$00226E`-$00231C (174 bytes): never read
- `$002328`-$002334 (12 bytes): never read
- `$002350`-$00235A (10 bytes): never read
- `$002370`-$00237C (12 bytes): never read
- `$002390`-$0023A2 (18 bytes): never read
- `$0023B8`-$0023C4 (12 bytes): never read
- `$0023D8`-$0023E0 (8 bytes): never read
- `$002400`-$00240C (12 bytes): never read
- `$002420`-$002432 (18 bytes): never read
- `$00243C`-$00244A (14 bytes): never read
- `$002476`-$002486 (16 bytes): never read
- `$0025E4`-$00267C (152 bytes): never read
- `$00269A`-$0026AA (16 bytes): never read
- `$0026B2`-$002AA6 (1012 bytes): never read
- `$002AB6`-$002AEA (52 bytes): never read
- `$002AFA`-$002B16 (28 bytes): never read
- `$002B26`-$002B86 (96 bytes): never read
- `$002B96`-$002BF6 (96 bytes): never read
- `$002C06`-$002C3A (52 bytes): never read
- `$002C6A`-$002C7C (18 bytes): never read
- `$002C92`-$002CA4 (18 bytes): never read
- `$002CD6`-$002CE6 (16 bytes): never read
- `$002F36`-$002F7C (70 bytes): never read
- `$002F9C`-$002FCE (50 bytes): never read
- `$002FE0`-$002FF6 (22 bytes): never read
- `$0031C6`-$003342 (380 bytes): never read
- `$003402`-$003434 (50 bytes): never read
- `$0034A2`-$0034A6 (4 bytes): never read
- `$003574`-$003714 (416 bytes): never read
- `$003756`-$00375A (4 bytes): never read
- `$00390C`-$003928 (28 bytes): never read
- `$003944`-$003960 (28 bytes): never read
- `$003A60`-$003A74 (20 bytes): never read
- `$003AA6`-$003ABE (24 bytes): never read
- `$003ACE`-$003ADE (16 bytes): never read
- `$003B3C`-$003B50 (20 bytes): never read
- `$003B5A`-$003B76 (28 bytes): never read
- `$003C8C`-$003CCA (62 bytes): never read
- `$003CDC`-$003CEE (18 bytes): never read
- `$003CF4`-$003D58 (100 bytes): never read
- `$003D64`-$003E58 (244 bytes): never read
- `$003F46`-$003F6C (38 bytes): never read
- `$003FAE`-$003FE4 (54 bytes): never read
- `$003FF6`-$003FFE (8 bytes): never read
- `$004006`-$004098 (146 bytes): never read
- `$0040A0`-$004132 (146 bytes): never read
- `$00413A`-$0042FE (452 bytes): never read
- `$004318`-$004326 (14 bytes): never read
- `$004330`-$00433E (14 bytes): never read
- `$00436C`-$00437A (14 bytes): never read
- `$0043B6`-$0043E2 (44 bytes): never read
- `$00442C`-$004454 (40 bytes): never read
- `$004644`-$0046CC (136 bytes): never read
- `$00474A`-$00478C (66 bytes): never read
- `$00486E`-$004898 (42 bytes): never read
- `$004926`-$004AB4 (398 bytes): never read
- `$004AD2`-$004B74 (162 bytes): never read
- `$004C5E`-$004CDA (124 bytes): never read
- `$004E2E`-$004E32 (4 bytes): never read
- `$004E4A`-$004E52 (8 bytes): never read
- `$004E7E`-$004EA8 (42 bytes): never read
- `$004F42`-$004F46 (4 bytes): never read
- `$004F4A`-$004F4E (4 bytes): never read
- `$0050BA`-$0050CE (20 bytes): never read
- `$0051D6`-$0051F2 (28 bytes): never read
- `$0052D0`-$0052F0 (32 bytes): never read
- `$0052FA`-$005306 (12 bytes): never read
- `$005370`-$005378 (8 bytes): never read
- `$005396`-$0053B4 (30 bytes): never read
- `$0053CC`-$0053D4 (8 bytes): never read
- `$005440`-$005450 (16 bytes): never read
- `$0054F0`-$005520 (48 bytes): never read
- `$005538`-$00555A (34 bytes): never read
- `$005586`-$0055C2 (60 bytes): never read
- `$0055D8`-$005606 (46 bytes): never read
- `$0056D0`-$0056E6 (22 bytes): never read
- `$005716`-$005722 (12 bytes): never read
- `$005746`-$005750 (10 bytes): never read
- `$005760`-$00576A (10 bytes): never read
- `$005774`-$00578C (24 bytes): never read
- `$0057A8`-$0057C0 (24 bytes): never read
- `$00582C`-$00583C (16 bytes): never read
- `$005884`-$0058A0 (28 bytes): never read
- `$0058AE`-$0058CA (28 bytes): never read
- `$0058E4`-$0058EE (10 bytes): never read
- `$00595A`-$005968 (14 bytes): never read
- `$005976`-$005990 (26 bytes): never read
- `$0059A8`-$0059EA (66 bytes): never read
- `$0059F8`-$005A12 (26 bytes): never read
- `$005A42`-$005A54 (18 bytes): never read
- `$005A6E`-$005A80 (18 bytes): never read
- `$005ADC`-$005AFA (30 bytes): never read
- `$005B26`-$005B82 (92 bytes): never read
- `$005B94`-$005BC4 (48 bytes): never read
- `$005BF0`-$005C20 (48 bytes): never read
- `$005C3A`-$005C7C (66 bytes): never read
- `$005C96`-$005CF2 (92 bytes): never read
- `$005D00`-$005D1A (26 bytes): never read
- `$005D32`-$005D74 (66 bytes): never read
- `$005DCC`-$005DDE (18 bytes): never read
- `$005DF8`-$005E28 (48 bytes): never read
- `$005E54`-$005E66 (18 bytes): never read
- `$005E84`-$005F0C (136 bytes): never read
- `$005F3C`-$005F4E (18 bytes): never read
- `$005F7A`-$005F98 (30 bytes): never read
- `$005FC4`-$00607C (184 bytes): never read
- `$00608E`-$00609E (16 bytes): never read
- `$0060BC`-$0060C2 (6 bytes): never read
- `$006208`-$006238 (48 bytes): never read
- `$0063AC`-$0063B6 (10 bytes): never read
- `$0063D2`-$0063E2 (16 bytes): never read
- `$006460`-$00646E (14 bytes): never read
- `$006542`-$006576 (52 bytes): never read
- `$0065A4`-$0065B4 (16 bytes): never read
- `$00664E`-$006690 (66 bytes): never read
- `$0066AC`-$0066E2 (54 bytes): never read
- `$00676C`-$006776 (10 bytes): never read
- `$006950`-$006952 (2 bytes): never read
- `$006A5A`-$006A68 (14 bytes): never read
- `$006AA8`-$006AC4 (28 bytes): never read
- `$006AE6`-$006C62 (380 bytes): never read
- `$006C8C`-$006CB0 (36 bytes): never read
- `$006CC2`-$006CC6 (4 bytes): never read
- `$006CCE`-$006CD2 (4 bytes): never read
- `$006CD6`-$006CE2 (12 bytes): never read
- `$006CEC`-$006D10 (36 bytes): never read
- `$006D5A`-$006DBC (98 bytes): never read
- `$006DEC`-$006DF4 (8 bytes): never read
- `$006E08`-$006E18 (16 bytes): never read
- `$006E3E`-$006E74 (54 bytes): never read
- `$006E88`-$006E9A (18 bytes): never read
- `$006EB2`-$006EBA (8 bytes): never read
- `$0070D2`-$0070E0 (14 bytes): never read
- `$0071E8`-$007202 (26 bytes): never read
- `$00721E`-$00723A (28 bytes): never read
- `$007296`-$0072D2 (60 bytes): never read
- `$007366`-$00737C (22 bytes): never read
- `$0073A6`-$0073EE (72 bytes): never read
- `$0073F6`-$007416 (32 bytes): never read
- `$007734`-$007736 (2 bytes): never read
- `$008584`-$008586 (2 bytes): never read
- `$0088F6`-$00890A (20 bytes): never read
- `$008948`-$00894C (4 bytes): never read
- `$008B3E`-$008B7E (64 bytes): never read
- `$008CC0`-$008CCA (10 bytes): never read
- `$008E3C`-$008E58 (28 bytes): never read (Obj_SpawnEnemyAfter's unreferenced OrQuit twin; $008E22 is code since 2026-10-30)
- `$008EA8`-$008EFA (82 bytes): never read
- `$008F36`-$008F3C (6 bytes): never read (the scripted entry falling into Obj_SpawnAfter; nothing names it)
- `$009030`-$00905E (46 bytes): never read
- `$00907C`-$0090AA (46 bytes): never read
- `$0090B0`-$0090C8 (24 bytes): never read
- `$0090FC`-$00915C (96 bytes): never read
- `$0091C8`-$0091F0 (40 bytes): never read
- `$00924A`-$009266 (28 bytes): never read
- `$00939E`-$0093A0 (2 bytes): never read
- `$00957C`-$009584 (8 bytes): never read
- `$009590`-$00959C (12 bytes): never read
- `$0096CA`-$0096DE (20 bytes): never read
- `$0097FE`-$009800 (2 bytes): never read
- `$009804`-$009808 (4 bytes): never read
- `$00980C`-$009810 (4 bytes): never read
- `$009818`-$009820 (8 bytes): never read
- `$009834`-$009848 (20 bytes): never read
- `$009864`-$009884 (32 bytes): never read
- `$0098C2`-$0098FE (60 bytes): never read
- `$00AA6E`-$00AA7A (12 bytes): never read
- `$00AB02`-$00AB24 (34 bytes): never read
- `$00AB3C`-$00AB6C (48 bytes): never read
- `$00ABB8`-$00ABCA (18 bytes): never read
- `$00AC3A`-$00AC66 (44 bytes): never read
- `$00AD02`-$00AD14 (18 bytes): never read
- `$00AE5C`-$00AE74 (24 bytes): never read
- `$00AE90`-$00AEA4 (20 bytes): never read
- `$00AEC8`-$00AECC (4 bytes): never read
- `$00AED8`-$00AEEC (20 bytes): never read
- `$00AEE4`-$00AEE8 (4 bytes): never read; VInt_BusyScrollTbl's case $20 (bra.w to mode $20's handler), left of $00AED8's span when its neighbours became functions (fix-spawn-table-gaps, 2026-11-17); no g_ScrollMode store names mode $20 (config/raw_reach_allow.txt)
- `$00AEF4`-$00AEF8 (4 bytes): never read
- `$00AF00`-$00AF04 (4 bytes): never read
- `$00AF56`-$00AF84 (46 bytes): never read
- `$00AFD6`-$00AFDA (4 bytes): never read
- `$00B0EA`-$00B1A4 (186 bytes): never read
- `$00C9FA`-$00CB7A (384 bytes): never read
- `$00D82C`-$00DAB4 (648 bytes): never read
- `$00E81A`-$00EAA2 (648 bytes): never read
- `$00FFC2`-$01010A (328 bytes): never read
- `$01015C`-$0101A8 (76 bytes): never read
- `$010692`-$0106A8 (22 bytes): never read
- `$010724`-$010742 (30 bytes): never read
- `$010782`-$0107B0 (46 bytes): never read
- `$010CE0`-$010D12 (50 bytes): never read
- `$010FB2`-$010FD2 (32 bytes): never read
- `$012540`-$012572 (50 bytes): never read
- `$0125D6`-$0126D2 (252 bytes): never read
- `$0127B8`-$0127D0 (24 bytes): never read
- `$012898`-$01289C (4 bytes): never read
- `$01297C`-$012A28 (172 bytes): never read
- `$012CF6`-$012D06 (16 bytes): never read
- `$012FE4`-$01300E (42 bytes): never read
- `$013032`-$013034 (2 bytes): never read
- `$013036`-$013038 (2 bytes): never read
- `$013060`-$01306A (10 bytes): never read
- `$013F92`-$013FA4 (18 bytes): never read
- `$0143B4`-$0143B8 (4 bytes): never read
- `$0143C0`-$0143CC (12 bytes): never read
- `$014EE2`-$014EE4 (2 bytes): never read
- `$015676`-$01569C (38 bytes): never read
- `$01590A`-$015916 (12 bytes): never read
- `$0172BE`-$0174F0 (562 bytes): never read
- `$017E3A`-$01803A (512 bytes): never read
- `$018534`-$018E44 (2320 bytes): never read
- `$019188`-$019474 (748 bytes): never read
- `$01CCA0`-$01D470 (2000 bytes): never read
- `$01DCFE`-$01E1CE (1232 bytes): never read
- `$02028E`-$02029A (12 bytes): never read
- `$020316`-$020354 (62 bytes): never read
- `$020C24`-$020C26 (2 bytes): never read
- `$020DC4`-$020DDA (22 bytes): never read
- `$021A10`-$021B26 (278 bytes): never read
- `$022DAA`-$022FA4 (506 bytes): never read
- `$027B1C`-$027C28 (268 bytes): never read
- `$02B09E`-$02B67A (1500 bytes): never read
- `$02BAF6`-$02C020 (1322 bytes): never read
- `$02C96E`-$02CC52 (740 bytes): never read
- `$02D54A`-$02D660 (278 bytes): never read
- `$039752`-$039776 (36 bytes): never read; a raw toy-chain base stub (lea Scr2_03975E,a2 / jmp
  Obj_SpawnEnemy, no static reference) and its script's start (child maker $039A88); added by
  corpus-p6 2026-10-30 (the span left between ws-tail's conversions)
- `$0399C6`-$039A06 (64 bytes): never read; two raw inline chain makers (jsr ToyChain_SpawnInline,
  10 links, records $039AA8 / $039AE0 in Tbl_039AA8) and their chain scripts (corpus-p6)
- `$039A88`-$039AA8 (32 bytes): never read; a raw 5-link chain maker (record $039B34) and its
  chain script (corpus-p6)
- `$039B50`-$039B78 (40 bytes): never read; a raw toy-chain base stub (lea Scr2_039B5C,a2 / jmp
  Obj_SpawnEnemy) and its script's start (corpus-p6)
- `$03B760`-$03B886 (294 bytes): never read
- `$03CCC0`-$03CCD4 (20 bytes): never read
- `$03DCDC`-$03DD02 (38 bytes): never read
- `$03DD0E`-$03DD32 (36 bytes): never read
- `$03DD34`-$03DD36 (2 bytes): never read
- `$03DD38`-$03DD42 (10 bytes): never read
- `$03DD44`-$03DD52 (14 bytes): never read
- `$03DD5A`-$03DD90 (54 bytes): never read
- `$03DF14`-$03E034 (288 bytes): never read
- `$03F5D8`-$03FB0E (1334 bytes): never read
- `$040944`-$040980 (60 bytes): never read
- `$047A66`-$047A72 (12 bytes): never read
- `$0491C6`-$0491D2 (12 bytes): never read
- `$054C86`-$055592 (2316 bytes): never read
- `$0FFB53`-$0FFB54 (1 bytes): never read
- `$0FFD3E`-$100000 (706 bytes): never read
- `$13FE8C`-$140000 (372 bytes): never read
- `$17FC40`-$180000 (960 bytes): never read
- `$19FDCE`-$1A0000 (562 bytes): never read
- `$1DDB62`-$1DDC6E (268 bytes): never read
- `$1DE95C`-$1DEBE6 (650 bytes): never read
- `$1DECB8`-$1DEDD0 (280 bytes): never read
- `$1DF000`-$1DF442 (1090 bytes): never read
- `$1E0F5A`-$1E10B2 (344 bytes): never read
- `$1E5022`-$1E5044 (34 bytes): never read
- `$1E5058`-$1E5066 (14 bytes): never read
- `$1E5090`-$1E50D6 (70 bytes): never read
- `$1E5FBA`-$1E5FFA (64 bytes): never read
- `$1E7929`-$1E799A (113 bytes): never read
- `$1E79D4`-$1E7AF8 (292 bytes): never read
- `$1E7B6C`-$1E7B70 (4 bytes): never read
- `$1E7D08`-$1E7D52 (74 bytes): never read
- `$1E7D8C`-$1E7EE4 (344 bytes): never read
- `$1E7EFC`-$1E8000 (260 bytes): never read

## P4 D2 divergences

The whole-corpus D2 (stepped == stepped-gen), one line per root cause: where it showed, the cause, the fix.

- **TAS sync 78,990, ram_crc32** (the player-select chooser after a continue, scene $9CE6; 8 bytes:
  the chooser's a4 block $AE1D-$AE43 and g_TintLevel/g_Pal1Tint*): `PlayerSelect_ToggleHero`
  (049B72.c) resumed the script at $049B7C instead of $049B7A, the address its `jsr $7FE.w` pushes
  (VM2_Enter pops it). The skipped word was the coroutine's return op, so the toggle ran into the
  confirm path (4 x Tint_FlashC1, the 300-frame timer cut to $40). Fix: the constant. Not caught
  earlier: the native never returns (it ends in VM2_Enter), so verify has no return to compare at,
  and the movie's first 6,000 sync points never toggle the hero. Check for every `jump_script`: the
  script address is the one after the native's own `jsr $7FE.w`.
- **scenario-level-skip sync 1,168, stepped stuck** (`stuck polling $A10005 in $00A400`, the
  V-int's busy-path pad read, every row before equal; d2k2): the pause (`Pause_Update`, 009C50.c)
  waits V-int by V-int for the section-skip code's presses. A generated wait site takes each V-int
  through spec.c's `wait_vint_here`, which sets the pads of the interval's next reference frame;
  the C's `IRQ_WAIT()` called `x68k_wait_vint` itself and kept the runner frame's pads, so the
  press never came. Fix: kit 0.9.4, `IRQ_WAIT()` is spec.c's `dk_irq_wait` (the template's
  spec.c + dk_mem.h). After it: stepped == stepped-gen, 12,541 rows byte-identical.
- **fuzz-05400-s1 sync 6,092, stepped stopped** (`entry $01A05E of the C function file $01A04C
  has no C`; d2k2): the game enters Boss_Harley_RumbleWhenFast past its speed test, but `recomp
  entries 01A04C` listed only the start (the call-site log has no computed call to $01A05E), so the
  C defined none. Found by bisecting the C files aside (01A04C.c alone). Fix: the entry in C,
  `Boss_Harley_RumbleCountdown` (asm label + prototype). `recomp entries` is evidence, not proof:
  a mid-function label that is a branch target can still be entered from outside.
- **D1 66/153, branch processes exit 0xC0000005 at init** (d2k2; not D2, but the same pass): every
  failing process said `RENDERER name=opengl`, every passing one `direct3d11`. Under the corpus
  pass's load D3D11 failed, SDL fell back to OpenGL and the NVIDIA driver crashed (event log:
  nvoglv64.dll). Fix: kit 0.9.3, runner_env sets `SDL_RENDER_DRIVER=software` (`--benchmark`
  never presents). D1 153/153.
- **fuzz-90000-s2 sync 92,341, row missing** (the whole-corpus D2 on c9ce8ed; d2k3): the stepped
  process `tas5-to-91341` died with `[handc] call of $009BF0 from C came back with A7 $FFFFFFFC
  (expected $FFFFFFF8) (C function $047C6A)` (Scene_LoopSkippable's Frame_End), 40 sync points
  before the run's end; played alone the run passed (92,381 rows = stepped-gen's), and a process of
  just fuzz-90000-s1 then fuzz-90000-s2 from frame 89,574 reproduced it. Cause: the branch snapshot
  holds the game fiber's stack, so the C frames come back with a restore, but handc_rt.cpp's depth
  of the calls from C in progress (each returns to RET_FROM_C + 2 x its depth) kept the value the
  previous branch ended at, so a new call took the return address of one the restored stack still
  waited for. Fix: kit 0.9.9, the depth (and g_left, g_fn_current) saved and restored with the
  snapshot (spec.c's branch hooks, `DK_HANDC`). The stop was reported as `[dkit] counters {...}`:
  `last_words` matched "miss" in `dispatch_misses`; 0.9.9 names the `[handc]` / `[stepped]` line.
- **The stepped machine's reach: no stepped run left stage 1's street** (cov14, 2026-10-28; found
  as 23 of ruling 7's 36 files failing D4's coverage, 21 from their first instruction, with D2
  passing). The coverage maps lose nothing: stepped-gen's tas map is 10,540 instructions against
  the reference's 26,589, and holds no pc of Stage1_RunGemExpo or stages 2-4. The stepped TAS stays
  in section $1100 (sync dumps, scratchpad cov14_ram.py: g_SectionId $1100 through sync 9,800,
  g_LivesSum 2 -> 1 by 5,560 -> 0 by 8,000, then the title loop $0010; the timed run's street
  callback $9D72 ends at sync 5,958), every fuzz run is its branch, and the scenarios are short. The one run
  that reaches the other stages, scenario-level-skip, never skipped a section on the stepped
  machine (it played the street out and stopped paused at 12,541; NOTES' "one-skip scenario never
  completed the code"). Cause: Pause_Update's wait site $9CBA. A code mismatch branches to
  Pause_CheatRestart, which compares the SAME press with B in the same frame; only a second
  mismatch spins until the V-int. The site took the V-int at the first loop-back, so the press was
  gone, and the code's first B always meets the player record first (a1 = g_Player1). Fix: kit
  0.9.15, `[recomp.sync] wait_vint` `spins` (the V-int on the spins-th loop-back in a row with no
  V-int between), `{ pc = 0x009CBA, spins = 2 }`, and the C's wait at the second mismatch in a row.
  After it: stepped-gen level-skip 20,906 of 20,906 syncs, its map 0 -> 20,937 instructions
  (19,244 of the reference's 19,357); stepped equal at every row once the C had the entry the run
  now enters, Coll_MoveGroundedBlockMap $010DF0 (a scene's terrain pointer, no reference run
  enters it). Measured (phase.py 4 --only D2 D4, private artifacts): D2 154/154 equal (10,175,742
  sync points, 0 stopped alike: level-skip no longer stops); D4 1,125/1,145 (P4.md's last record 1,086, + 13 by D2 with 0.9.14),
  stepped-gen credits 661 instructions (was 67), 31 of the 36 V-int files checked by D2 (was 13),
  20 files with uncovered instructions (first 006776). Still short: the stage cards (StageCard_Stage2-4: the stepped level-skip enters the
  next section one sync late and the interval's OR of the code presses skips the card), and the
  TAS's own desync in the street.
- **The stage cards: the scenario's unpause press, not the code's** (cov15, 2026-10-29). The
  skip code ends the pause itself (Pause_Update's Scene_FadeOutExit, then `rts`), so level-skip's
  "unpause" S x2 30 frames later falls in the next section's load: before stage 2, frames
  3688-3689 of the D1 interval sync 2988 (f3669) -> 2989 (f3695), which has no sync point. The
  reference ignores it; on the stepped machine the interval's first V-int reads the OR of its
  frames (stepped_pads), so Start is newly pressed when the Batmobile intro's Scene_LoopSkippable
  starts: g_SceneResult 1, Scene_RunList drops the card, stepped-gen goes 9d54 -> 9d72 at sync 2990
  (the timed run: 9d08 at 2989, the card at 3130). The OR rule loses nothing here (the press is
  also in the interval's extras, frame by frame); it moves a press to the interval's first V-int,
  which is what keeps the movie's Start tap in a 29-frame load, so it stays. Fix: that press is
  no press in sections 4, 9, 15 (scenario only; the reference plays the same, its coverage files
  unchanged). After it stepped-gen's scene changes are the timed run's at every sync, the cards
  included. Frame_End's $009C3A path (2 more V-ints a frame) needs g_SlowMotion, and
  Stage2_RunLift's $032FB4 g_FreeCam: both only toggled by Debug_PauseMenu (dead), and verify
  cannot run past either file's V-int wait, so Unreachable rows. Measured (`tools/phase.py 4`,
  shared artifacts): D2 154/154 equal (10,175,742 sync points); D4 1,130/1,145, all 36 V-int
  files checked by D2, 15 files with uncovered instructions (first 006776), none of them a
  V-int file.

## P4 stepped-runner audio (D8 playtest, 2026-10-29)

- **"Misses a beat occasionally"** on the stepped and stepped-gen runners, menu and stage 1; the
  timed runner clean. Not delivery: on the real device (`GENESIS_AUDIO_FLUSHLOG`, engine
  9916086) both runners carry 3,732 PSG-rate samples (223,721 Hz) in every presented frame, 0
  DRC-bridge underruns, 0 conceal events, idle 1,800 frames and the movie's first 3,000. The
  content: the frame-synchronous machine runs the Z80 for a whole frame at the start of each
  runner frame (then re-bases its clock), plus 8 lines at each 68000 bus release in the interval
  (this game: 3 releases in 2,997 of 3,000 intervals, ~82,000 master clocks); the mixer drains
  one wall frame after the interval. The releases' FM/PSG writes were stamped at the start of
  that drain, ahead of the frame's own earlier Z80 time: ~24 lines of each frame's Z80 time
  played a frame early, out of order (`GENESIS_AUDIO_APPLIEDDUMP`: 118,821 writes in 1,385 of the
  movie's first 3,000 drains; menu 187/400, stage 1 1,198/2,600; timed 0). Fix (engine
  fsync-audio-stamps 9916086): those writes stamped one Z80 frame on, so the mixer defers them
  to the next drain in execution order; after: 0 out-of-order writes, stepped and timed alike.
  Output only: the movie's 130,055 stepped sync rows and scenario-level-skip's 20,906 are
  unchanged (z80_crc32 included), stepped == stepped-gen; D1 movie 130,772 frames equal; the
  timed runner's WAV byte-identical. Hear it: `uv run dkit genesis recomp build`, then
  `uv run tools/playtest.py --handc`.

## P4 D4 mismatches

The whole-corpus verify's mismatches (kit 0.9.6+ compares the calls an interrupt landed in), one
line per root cause: function, symptom, cause, fix. `DKIT_VERIFY_DIFF=1` (kit 0.9.9) prints each
mismatch in full (RAM bytes, both register files, lowest A7, both instruction paths).

- **Boss_TurretPod_RunTurrets $029082** (TAS call 2: RAM $FF9DDD g_SprNodeAlloc, generated $38, C
  $6E, 3 display nodes fewer): Boss_TurretPod_RunTurret's first instruction is `beq` on the Z its
  caller's `move.w` left, and its prototype took no condition codes, so the C caller's call left Z
  stale and the generated RunTurret skipped turrets. Fix: RunTurret takes `ccr` (regs(..., ccr)),
  the caller passes `tst.w` of the handle. It was the only entry in the tree whose first
  instruction reads the condition codes (a scan of src/f/ labels).
- **Boss_Hatter_LittleHatDropLife $03D9AE** (d4 generated $01A601A6, C $01A60000): its registers
  were inputs only, so what SpawnAction_Run left never reached the `jump_script` into VM2_Enter.
  Fix: d0-d7/a1 in/out.
- **Boss_Hatter_RoadHazardPlace $03DE80** (d1 generated $FFFFF594, C $FFFF9E2E): the C never set
  a1 to the row (`lea`/(a1)+ x3 leave it on the scale word), which Spr_AddBack copies to d1. Fix:
  `*a1 = &row->scale`.
- **Flight_FrameBeamShip $033A88** (d0 generated $00000010, C $00700010): after Span_BuildCircle
  (`moveq #0,d0`) the asm's `move.w g_BeamRadius,d0` has upper word 0; the C kept its own. Fix:
  `*d0 = g_BeamRadius`.
- **Boss_Hatter_StepRoadSchedule $03DC94** (TAS calls 1442/1522, the two road spawns: $40(a4)
  generated $0003F58E, C $0003DC94): same-file calls are direct C calls, so when
  Boss_Hatter_RoadCmdNext jumps to a command handler, the handler runs with the registers the
  OUTER slot leaves; a0 (the schedule pointer past the command) was a clobber of
  StepRoadSchedule, so the handler stored the entry's a0 (the native's address). Fix: a0/a1 are
  StepRoadSchedule's outputs. Check for any C chain that ends in `dk::jump`: every register the
  target reads must be an output of the outermost function of the chain.
- **Hitbox_SpawnAtContact $01088A / ToyChain_Update $0371E6** (RAM $FFFF4E, 2 bytes below
  stack_low $FFFF50: generated $61, C $00): dead stack the mask missed, not the C. The generated
  run reached A7 $FFFF4C (its movem of 7 registers; the C, 28 bytes shallower, never wrote there),
  but D1 measured $FFFF50 as the corpus's lowest A7: `g_xa7min` is machine state in the branch
  snapshot, so each restore erased the lowest A7 of the branch just played. Fix: kit 0.9.9 keeps
  the branches' lowest across restores; D1 re-measured $FFFF4C (processes tas5-to-109800,
  tas6-to-102600); `[recomp.sync] stack_low` = $FFFF4C.

## Phase 5: the raster on the frame-synchronous machine

Ruling R3 (2026-10-29). Engine `fsync-raster` (crabsmack/segagenesisrecomp bd92ab6, its core
crabsmack/m68k-recomp-core 75cda60), switched on by `GameSpec.fsync_raster` (spec.c
`DK_FSYNC_RASTER`, 1; other games keep the old machine).

- **The timed machine, measured** (`GENESIS_XTRACE`, the H-int vector's entries by master clock /
  3,420): TAS 40000 and 77200: 224 H-ints a frame, the first ~10 lines after the V-int (the line
  223/224 counter fired while the handler had H-int off, the V-int's `HInt_Arm` re-enables it with
  the latch pending: taken at its RTE, writing line 0's V-scroll), the others at the ends of lines
  0-222 (the 224th turns H-int off again). TAS 20000 (Joker balloon): 2, lines 100 and 201. TAS
  21210 (lift floor): 1, line 175 ($AF). TAS 33000 (arena split): 2, lines 83 and 167.
- **The model.** After each V-int (at the wait site, this game's `fsync_vint_at_wait`) the
  display is drawn line by line: the H-int counter (reg 10) reloads at the display's start, fires
  at the end of each line (and vblank's first, as the timed `hint_tick`), and with reg 0 bit 4 on
  and the mask below 4 the handler runs before the next line. A latch left pending while H-int
  was off is taken at the display's start. The 68000 side: the main program is parked at its wait
  (SR $2004 or $2014 there), so each H-int interrupts that context like the V-int just before
  (PC and SR pushed on the main program's A7, the RTE returns there). The V-int is latched before
  it is taken so its acknowledge leaves the H-int latch alone. Only reg 0/10 writes and the mask
  drive it, and `--no-render` changes nothing (the H-ints run without the picture). Left out: an
  H-int the main program unmasks or enables between displays waits for the next display.
- **RAM code**: the untimed builds now generate the `[recomp] ram_code` overlays (no clock, the
  template check kept) and the stepped runners run them (glue.c), so the RAM handler at $FFE560
  and its jmp stubs (to the C entries 01C468 / 024526 / 0245E0 in the stepped runner) run with 0
  interpreted instructions. The RAM handler is still generated code (phase-4 ruling 6); the C
  raster callback R3 names is the native port's to write (P3 forbids generated code there).
- **Seen by the stepped machine** (`GENESIS_FSYNC_HLOG`): of the corpus only
  scenario-level-skip reaches raster scenes (the movie desyncs on the frame-synchronous machine
  and never leaves stage 1): 4,583 of its 23,052 displays take H-ints, 4,287 x 224 (first at the
  display's start, last at line 222, as timed), 295 x 1 at line 175 (the lift floor), 1 x 224
  (lines 0-223, the first).
- **D2 on it** (both stepped modes, raster on): `stepped-corpus` 154/154 runs played in each mode
  (10,175,742 sync points each), 0 dispatch misses, 0 interpreted instructions; `stepped-compare`
  154/154 equal. Against the rows from before (engine 9916086): 153 runs identical in both modes;
  scenario-level-skip differs in 3,960 of its 20,906 rows from sync 3,352 (ram_crc32 3,779 rows:
  the handler's operand and branch byte at $FFE562/$FFE56D, vsram_crc32 3,366, vram_crc32 181),
  the same in both modes.
- **The picture.** The stepped corpus can't reach the movie's raster scenes, so a timed run's
  state was put into stepped-gen (`GENESIS_SYNC_STATE_OUT` at timed sync S, `GENESIS_SYNC_STATE_IN`
  at stepped sync 2,000 of the stepped movie, both in scene $9D72's loop; then the movie's sync
  intervals S+1..), and the timed and stepped pictures compared frame by frame (stepped runner
  frame 2,000+j = TAS frame F+j, so no frame-late shift here: this game's VDP writes are the
  V-int's) and line by line (`GENESIS_LINELOG`: VSRAM, the line's H-scroll words, regs 2/4/11):

  | TAS F (sync S) | scene | raster on: pixels | raster on: lines | raster off: pixels |
  |---|---|---|---|---|
  | 20000 (19741) | Joker balloon split + restore | 11/11 frames identical | 224/224 | 0/11 (truck deck missing below line 101) |
  | 33000 (32684) | Stage2_ArenaHIntSplit | 11/11 | 223/224 (line 84: a VSRAM column the timed handler is still writing as the beam passes; no pixel differs) | 0/11 (floor rows 168-223 wrong) |
  | 40000 (39667) | 224-line V-scroll | 11/11 | 224/224 | 3/11 |
  | 77200 (76859) | stage-2 boss spans | 11/11 | 224/224 | 0/11 |

  (The first frame after the load differs: the load's own seam.) Screenshots, line logs and the
  four states: `artifacts/wt/engine-raster/pictures/`.
- **A raster-free picture is unchanged** (`--hash-frames 1`, rendered, stepped-gen before and
  after): the movie's 130,055 runner frames all equal (it takes no H-int on this machine);
  scenario-level-skip: 3,744 of 20,906 frames differ, every one a frame with H-ints (0 differ
  without one; 48 raster frames happen to look the same).
- **Negative control** (a build with `-DDK_FSYNC_RASTER=0` on the new engine): both stepped
  corpora's 154 runs identical row for row to the rows from before (10,175,742 each), and
  stepped-compare 154/154.
- **D1** (the timed runner, which now also carries the diagnostics): `recomp corpus` 154/154 runs
  equal to the reference every frame (1,769,814 frames in 17 branch processes, 10 min), and
  their 154 sync.csv files byte-identical to the shared D1 traces (10,175,742 rows).
- **Cost** (one process at a time, `--no-render` as the corpus runs them; before -> after): the
  movie (130,055 runner frames, no H-int on this machine) stepped-gen 1,722 -> 1,749 fps, stepped
  2,245 -> 2,551; scenario-level-skip (20,906, raster in 4,583 displays) stepped-gen 2,726 ->
  2,639, stepped 2,711 -> 2,837; rendered stepped-gen level-skip 792 -> 906. All within run-to-run
  noise (~10%): 224 calls of a 5-instruction handler a frame cost nothing measurable; the movie
  takes none. The native port's cost is P7's to measure on its own movie trace.
- **Left for the port**: the RAM handler as a C raster callback (R3; P3 forbids generated code in
  the native objects): the engine's raster calls the H-int vector, so the native build needs its
  own route from $FFE560 to C (the handler's live operand and branch byte, the three stubs).
- **Kit**: `dkit artifacts private` links `recomp/traces` to the shared directory, so a D1 rerun
  under a private DKIT_ARTIFACTS rewrites the shared traces; this check used its own directory
  (`artifacts/wt/engine-raster-d1`, inputs/runs/r3 linked only).

## P5 kickoff: the contract (kit 0.10.0, 2026-10-29)

- **The native runner** (`batmanrobin_native`, recomp/CMakeLists.txt from the kit template): the
  hand C in the headers' DK_NATIVE branch (its own handc library, build/bridge-native), on the
  frame-synchronous machine, the stepped code set and the engine linked around it (until P6). Its
  sync rows come only from game/runtime/export.cpp's image (spec.c DK_NATIVE). At the kickoff the
  exporter copies all 65,536 work-RAM bytes from emulated RAM every export (`copied_bytes` 65,536,
  `native_bytes` 0).
- **Exporter self-test**: `DKIT_EXPORT_SELFTEST=FF816D` (a byte stepped-gen holds 0 at sync 401
  and 10 at 402, dumped with DKIT_SYNC_DUMP) made the native movie trace differ from stepped-gen
  from sync 402 on, in ram_crc32 only. Without it: equal (the first 3,000 sync points, then the
  corpus: P1).
- **Object scan** (`dkit genesis recomp scan`), 1,145 game objects: cpu68 1,001 references in 849
  files (dk::jump 721, dk::jump_script 94, dk::pop_long 62, dk::push_long 58, dk::drop_return 15,
  ...), emu_ram 1,001 in 575 (g_ram_ffff 575, g_ram 426: dk::host/addr inlined), bus 93 in 49
  (platform.h's inline m68k_write16/32, read8...), rom_image 426, generated 0. Hardware literals:
  21 in 4 files (platform.h 18; 0091F2.c, 020A48.c, 04062A.c: 0xA00000u, the Z80 RAM base). Link
  map: 16,332 forbidden (recomp_runtime 16,062: the stepped code set's dispatch, bridge.obj's
  slots and stubs, handc_rt; cpu_core 204; interpreter 63; emu_ram 3), gpgx 0.
- **Picture**: the movie's 130,055 sync points with `--fb`: native == stepped-gen at every row.
- **Windows package** (14 s): 11 files; the exe and everything else 0 ROM windows except SDL2.dll
  (4: SDL's table of 125 zeros then 01..83, which the ROM holds at $1E7188); smoke: the ROM runs
  600 frames (exit 0), the ROM with byte $1F0 flipped is refused by the SHA-1 check (exit 2).
  Queued in QUESTIONS.md (the criterion stays as written until ruled).
- **Linux (WSL, GCC 15.2)**: the native runner builds from the same sources once the runners get
  `_GNU_SOURCE` (the engine's ram_views.c calls memfd_create; GCC 15 refuses its implicit
  declaration); 0 ROM windows (SDL2 is the system's); smoke passes (exit 0; changed ROM exit 2).
  The movie trace stopped at sync 685: "no sync point for 30 s ... last game function entered
  $00A20C" (BgTask_Resume), its 685 rows equal stepped-gen's. MSVC's native runner ran past it.
  Solved (p9-gcc, below): the movie now equals stepped-gen at all 130,055 sync points.
- **P9's GCC hang: indeterminate condition codes into SR** (p9-gcc, 2026-10-29). The stop moved
  from run to run (sync 685, 736: 3 of 5 at 736), so: uninitialized data, its garbage moved by
  ASLR. Bisected with prints in the WSL build, not by diffing states: after BgTask_Resume the SR
  was $2FE4 / $27E4 (bits outside $A71F) while Lz_Decompress had returned with $2004; a trap in
  `dk_flags_set` for an SR with bits $58E0 named the writer: Op4_Spawn ($005606), SR $2019 ->
  $29E8 from Flags bytes 80 08 7A 00 00 (stack garbage: pointer bits). Cause: `dk::Flags
  spawn_ccr;` passed uninitialized as VM4_SpawnChild's ccr input; the bridge's stub loads a ccr
  input into SR (`dk_flags_set`: `f->x ? 0x10 : 0`). MSVC tests the bool; GCC takes a bool to be
  0 or 1 and shifts it (`b << 4`), so a garbage byte $7A set SR bits 5-11: interrupt mask 7. The
  V-int is taken only below mask 6 (exact_sched.c x68k_wait_vint), so Frame_End's
  `while (!g_VIntDone) IRQ_WAIT()` spun forever after BgTask_Resume, the last function entered;
  it was never in the background task. Fix in the C (acd0bde): the seven `dk::Flags` locals a
  call read before anything set them now `{}` (005606.c x2, 0055C2.c: VM4_SpawnChild; 006A68.c:
  Obj_UnlinkFree; 015730.c: Spr_AddSorted; 004E52.c, 0065B4.c: the first Hitbox_FindNextBox of
  their loops). No callee reads these inputs (verify passed on MSVC with garbage); the gate's D4:
  2,110 functions, 0 mismatches. With a trap for any Flags byte > 1 in `dk_flags_set`, the full
  movie ran clean in WSL twice; its 130,055 sync rows are byte-identical to stepped-gen's (both
  runs). Also (9c805ba, kit 0.10.1 template): GCC/Clang builds compile with `-fwrapv
  -fno-strict-aliasing -fno-delete-null-pointer-checks` (kit porting.md point 8); not the cause.
  For p3-flow's native flow runtime: a ccr INPUT is UB to read when the caller left it
  indeterminate; reading the bytes (`memcpy` to unsigned char, `!= 0`) in the runtime's flags load
  would make such a slip harmless in both compilers, and a lint rule (a `dk::Flags` local passed
  before it is set) would catch it, but that tightens D4's lint-clean (a criterion change: queued
  in the kit CHANGELOG, not done).

## Phase 5: the platform layer (P4, 2026-10-29)

- **Where**: `recomp/platform/genesis_platform.h` (the hardware names and the API, namespace `hw`)
  and `recomp/platform/platform_bus.cpp` (the backend on the engine's emulated bus).
  `recomp/game/include/platform.h` is now a one-line include of it; the C files did not change
  (`hw::` 52 files / 366 uses, as before).
- **The API is shaped like the ports, addressed by value**: `hw::port_read8/16/32(port)` and
  `hw::port_write8/16/32(port, v)`, one call per 68000 access, in the original's order, width and
  address. The port is the 68000 address of the register (the `hw::VDP_DATA` ... `hw::Z80_RAM`
  names) because the game keeps ports in registers: of 250 port calls, 154 pass a value the C holds
  (the tile-queue writer's a0/a1 = VDP control/data, `ctrl`/`data`/`vdp_ctrl` parameters, the sound
  mailbox's Z80 RAM addresses `request + b`, `channel + SNDCH_ON`, the pad port `port`). The
  calls are out of line (the backend's object), so no game object references `m68k_read*/write*`.
  Naming trap: the scan's bus regex is `^(dev|io|z80|vdp|psg|ym)_(read|write)` on unqualified
  names, so a C-linkage `vdp_write16` would count as bus; the namespaced `hw::port_*` doesn't.
- **The decode both backends share**: `hw::device_of(port)` (constexpr, 24-bit): VDP data
  $C00000-3, control $C00004-7, H/V $C00008-F, PSG $C00011/13/15/17, I/O $A10000-1F, Z80 bus
  request $A11100-1, reset $A11200-1, TMSS $A14000-3, Z80 RAM $A00000-$A01FFF; anything else is
  `none`, and the bus backend makes that a loud `dk::fatal`, so P1/P2's passes over the emulated
  runners prove that list complete for P6's native backend. Measured: no fatal on any of the 154
  corpus runs on stepped, verify (1,769,814 frames) or native: P1 154/154, P2 (D2 154/154, D4
  1,145/1,145, D6) and P4 pass on the platform layer (`phase.py 5 --only P1 P2 P4`, caches cold).
- **Accesses by named port** (static; the register-held ones are VDP control/data and Z80 RAM):
  VDP control w16 x18, w32 x23, r16 x2 (the status: the command latch reset, the DMA-busy wait);
  VDP data w16 x4, w32 x9; PSG w8 x1; I/O version r8 x2, control/serial-control w8 x6, the
  cold-start r32 ($A10008) and r16 ($A1000C); Z80 bus request w16 x14, r8 x6 (the grant poll);
  Z80 reset w16 x8; TMSS w32 x1; Z80 RAM r8/w8 (each byte written and re-read until it matches).
- **The backend seam (P6)**: recomp/CMakeLists.txt's `game_handc(... PLATFORM <backend>)` links
  `platform/platform_<backend>.cpp` into each hand-C library: `bus` (default) for stepped, verify
  and, until P6, native. P6's native machine adds `platform_native.cpp` (the same six functions,
  switching on `device_of`: the VDP model genesis_vdp.c with DMA through its callback, the Z80
  core with its RAM and bus/reset lines, the PSG, the pads) and passes `PLATFORM native` for the
  native library. The interrupt mask (`dk::set_interrupt_mask`) and the V-int wait (`dk_irq_wait`)
  are still the 68000 machine's and are cpu68 in the scan: they belong to P3's native flow runtime
  (R8), then the platform.
- **The three `0xA00000u` literals were not hardware**: 160 px in 16.16 (Fx_SpawnAtScreenCentre's
  camera + $A00000, the rubble rain's screen centre, the road camera's half screen). Written as
  `160u << 16` (and the $700000 beside the first as `112u << 16`); the lint already let them pass
  (REGION_BASES), the P4 scan counts every literal in the hardware ranges.

## Phase 5: the native flow runtime (P3, ruling R8, 2026-10-29)

- **What it is**: `recomp/game/runtime/flow.cpp` (namespace `dk::flow`, 299 lines) with
  `runtime/dk_flow_rt.h`; the native bridge from kit 0.10.2 (`bridge --mode native`); the adapter
  `runtime/flow_host.cpp`; the H-int RAM code in C `runtime/hint_raster.cpp`. The C files did not
  change: dk_flow.h declares its calls in `dk::flow` under DK_NATIVE with `using flow::jump;` etc. in
  `dk`, and dk_mem.h's IRQ_WAIT is `dk::flow::irq_wait()` there.
- **The runtime** keeps exactly the phase-4 runtime's rules (handc_rt.cpp and the engine's drain
  loop), on its own state: a register record (d0-d7, a0-a7, SR, USP); A7 and the stack as data at
  the original addresses (through the platform's memory hooks, which flow_host.cpp implements with
  dk_mem.h's `AT68(u16/u32, a)`, so the stack moves into p5-arena's arena with the rest of RAM);
  run loops on the game's own stack (one per call from C, per interrupt, and the reset's), each
  dispatching its pending address through the bridge's table `dk_code_table` (2,115 entries,
  binary search; a RAM address goes to `dk_ram_code`; anything else is fatal); a call from C
  pushes RET_FROM_C + 2 x depth and waits for it; the slot's RTS pops; an RTS to an outer call's
  token ends every loop in between and those calls throw `dk::Unwind` (caught by the slot of the C
  function the code returned into), an RTS nobody waits for runs on there; `dk::jump` sets the
  innermost loop's next address. Interrupts: SR to supervisor with the mask at the level, PC and SR
  pushed, the vector's handler in a loop of its own, which must end with an RTE back to the pushed
  PC. Branch plans: its state (record, loop head, depths, last RTS/RTE) saved and restored with the
  snapshot (spec.c's branch hooks), as handc_rt.cpp's depth was.
- **Condition codes into the record from the bytes** (after p9-gcc's hang): the native bridge's
  `dk::bridge::flags_set` reads each `dk::Flags` member as an unsigned char and tests `!= 0`, so a
  Flags left uninitialized can set at most one condition code, never SR's mask (kit 0.10.3).
  Re-measured on main 7965948 + this (the C's Flags{} fix in): P1 154/154, P2, P3 pass.
- **R9 as fatals in the runtime, not in the C**: an RTE outside an interrupt (BgTask_Resume's fake
  RTE to a parked task, 00A20C.c:46) and an interrupt's RTE to anywhere but where it interrupted
  (VInt_Handler's parking branch, 007736.c:93) both stop the run with a message naming R9. Neither
  happens on the 154 corpus runs (P1 passes with no fatal).
- **The engine under it, until P6** (flow_host.cpp, the one object still on the 68000 machine): the
  reset copies the engine's reset registers into the record and runs $000300; each dispatched entry
  calls `x68k_entry` (the sync points; the record is copied into g_cpu at the sync PC for the
  dumps); IRQ_WAIT copies the record's SR into g_cpu.SR and calls spec.c's `dk_irq_wait` (the
  wait-site pads, the frame-synchronous V-int and raster); the engine's interrupts come back
  through **GameSpec.take_interrupt** (engine native-flow 8cbd519: the machine acknowledges and
  keeps positions and depth, the game does the exception) and the record's SR is copied back after
  each. g_cpu.SR's mask is the only CPU state the engine still reads (the waits, the raster's H-int
  test). The engine's dispatch table (the stepped code set's) still links `func_ADDR` for every C
  address: `bridge-native/engine_stubs.cpp`, each a fatal, never entered.
- **The H-int raster as C** (R3, phase-4 ruling 6): `dk_ram_code($FFE560)` recognises the two
  shapes as the engine's overlays do (shape A: every byte of Tbl_HIntHandlerSrc $00B20C but the
  live $FFE562-3 and $FFE56D; shape B: `jmp (xxx).l`), does what the code does (A: the line table's
  word to the VDP data port, the operand's low byte +2, on a carry the bcs.s target by the live
  displacement: $FFE570 next page + displacement $0E, or $FFE57C VDP reg 0 = $04) and its RTE, or
  jumps to B's target through the table (01C468 / 024526 / 0245E0). Negative control: the data
  word XOR 1 made native scenario-level-skip differ from stepped-gen in 3,604 of 20,906 rows from
  sync 3,534 (vsram_crc32 first); reverted: 0 rows differ.
- **Measured** (kit 0.10.2, engine 8cbd519, private artifacts, caches cold): P3 0 cpu68 symbols in
  0 of 1,145 game objects (1,001 in 849 before; the flow runtime: 1,001 uses, dk::flow::jump 721,
  jump_script 94, pop_long 62, push_long 58, ...; flow.obj references nothing of the 68000); P1
  154/154 runs equal at every sync point (10,175,742); P2 passes (D2 154/154, D4 1,145/1,145, D6).
  The native movie's first 3,000 sync points equalled stepped-gen's at the first build (1,597 fps).
  The gate: 66 s. Link map forbidden 7,449 (16,353 before: the bridge is no longer the 68000 call
  layer; left: the engine's core 217, interpreter 64, runtime 188, generated 6,977 = the stepped
  code set and engine_stubs.cpp's func_ names, emu_ram 3).
- **Left for P6**: a native frame-synchronous machine (the V-int at the wait, the raster's H-ints
  calling `dk::flow::interrupt`, the sync point at `entry(DK_SYNC_PC)` at interrupt depth 0, the
  Z80 frame), replacing flow_host.cpp's engine calls; then the stepped code set, engine_stubs.cpp
  and handc_rt.cpp leave the native link. The runtime itself needs no change for it.

## Phase 5: the native arena (P5, ruling R1, 2026-10-29)

- **Where**: `recomp/game/runtime/arena.cpp` (the arena, the tags, the slow paths, the image, the
  census) and `recomp/game/include/dk_mem.h`'s native branch (`DK_MODE_NATIVE`: `dk::mem<T>`,
  host/addr over the arena); the exporter `runtime/export.cpp`; the DMA route in
  `recomp/platform/platform_bus.cpp`; spec.c (power-on, branch-plan share). From xmen2's
  native_rt.cpp / xmen2_mem.h (its R1 (b)), the same tag encoding.
- **Layout**: the 64 KB of work RAM at their offsets, mapped twice back to back (`dk_arena` =
  $00FFxxxx, `dk_arena_ffff` = +$10000 = $FFFFxxxx), so both alias forms convert back exactly
  (`dk::addr`) and one range check covers both; `dk_tags[65536]`: 0 = a plain byte (the 68000's
  value), `W<<4 | i` = byte i (68000 order) of a W-byte unit stored whole in host order. Stored
  pointers stay 32-bit 68000 values (`dk::bptr` over a `mem<uint32_t>`). globals.h (generated, names
  `g_ram_ffff`) is untouched: the native branch defines `g_ram_ffff` as `dk_arena_ffff`, so every
  global is at its address in the arena.
- **The types**: in native, `s8 u8 s16 u16 s32 u32` are all `dk::mem<T>` (bytes too, as xmen2: a
  byte store into a word unit must split it). A read at the stored width (`dk_tags[off] == W<<4`)
  is a host load; anything else is assembled from the 68000's bytes (`arena_load_slow`); a store at
  another width first turns the cut units into plain bytes. ROM (R2) reads big-endian through the
  same types; host memory (C locals of game types) is host order; a null reference or a store
  into the ROM image is fatal. Copying a record copies field by field (the copy constructor).
- **C changes (both modes compile, R4)**: 16 files. `memset`/`memcpy` on game memory became
  `dk::mem_set` / `dk::mem_copy` (12 files: plain memset/memcpy in PC mode; in native the 68000's
  bytes, stored plain, so a zero fill of half a long unit clears the right half); raw byte views
  of game memory became `u8` (0063E2 Op4_SetAnimRateRandom's script operand bytes, 1E7AF8
  Snd_SendSfxBody's slot bytes to the Z80, 011314's `uint8_t *record` parameters). Nothing else
  needed a change: 1,145 files compiled in native with `u8` a class on the first try but these.
- **The export** is the per-unit byte swap: 8 bytes at a time when their tags are plain, four word
  units or two long units (lane swaps), byte by byte by the tags otherwise (`dk_arena_image`).
- **Who else reads work RAM** (until P6 drops the engine's machine):
  - the VDP's 68000 -> VDP DMA (the engine's `dma_68k` reads its emulated RAM, `gbus_peek16`):
    the platform backend, in native, stages the source words from the arena into the emulated
    RAM, big-endian as the bus delivers them, at each control-port word that can start one (CD5,
    reg 1 DMA on, reg 23 a 68000 transfer; the stepped machine runs the whole transfer at its
    start). A long control write is its two word writes (as glue.c m68k_write32 does it in exact
    mode), so a DMA set up by the first word's register write is staged with it. P6's native VDP
    reads the arena through its DMA callback (`dk_arena_read_be`).
  - the 68000 stack and the H-int RAM code: the native flow runtime (P3, flow_host.cpp) and the
    raster in C (hint_raster.cpp) reach them through dk_mem.h's `AT68(u16/u32/u8, a)`, so they live
    in the arena like everything else (a stack long pushed is a host-order long unit; the export
    swaps it; the stack area stays masked in the CRC as phase 4 masks it). Before the flow runtime
    landed (b68f197, measured on 7608a14) the engine's own 68000 still pushed return addresses and
    exception frames in its emulated RAM and its overlay ran the RAM handler, so those bytes
    ($FFF800-$FFFFFF above the last global $FFF704, and $FFE560-$FFE585) were tagged ENGINE and
    read through to the emulated RAM by both sides, counted as copied (2,086 bytes); without that
    share the first frame missed a dispatch at $000000 (the C read return addresses from the arena
    while the engine pushed them in its RAM). Removed on the flow runtime: copied_bytes 0.
- **Census of the layout-dependent sites** (`DKIT_ARENA_CENSUS=<file>`: every access at another
  width than the bytes were stored, by C function, width and the first byte's tag; the movie,
  130,055 sync points, native): 794 sites in 327 functions, 6.9 M slow loads and 0.87 M slow
  stores (~60 a frame), 146 K block copies/fills. By shape:
  - word reads of bytes stored one by one (4.5 M; L2 on plain bytes): buffers a byte writer fills
    and word code reads: Lz_Decompress's output and the scene buffers read as map/tile words, the
    DMA queue's entries, the sprite list nodes Spr_BuildSAT walks (3.35 M of all slow accesses,
    `$FF9DFA`), the camera's view words;
  - halves of long units read as words (1.09 M; L2 on a long's +0/+2): a long pointer whose low word
    is used as a handle (`move.w 2(a0)`), 16.16 positions read as their integer word (Camera_
    UpdateView, Fade_Step, the H-scroll table writers $00AF10);
  - bytes of word/long units (L1/S1, 0.57 M): flag bytes inside longs (Fade_Step's palette levels,
    Snd_SendSfxBody's six slot bytes to the Z80, Frame_End's counters), g_SprCountLo (the low byte
    of g_SprCount), the attribute bytes of sprite nodes;
  - longs over two words or bytes (L4/S4 on plain or word units, 1.1 M): x.w/y.w pairs moved as
    one long (Op2_StepVelocity, Shake_Update, 012DBA), Mem_Clear and the object initialisers
    clearing with long stores over word fields;
  - block copies and fills (C, 146 K): the HUD's tile copies (HUD_DrawLifeRing, HUD_DrawWeaponBar,
    HUD_DrawScoreDigit), the clears in Obj_InitSpawned / ArtCache_Init.
  Every one reads what the 68000 read (the tags assemble it); the cost is ~60 slow accesses a
  frame: the movie plays at ~1,270 fps native with the arena (stepped-gen's own ~1,700).
- **Measured** (`phase.py 5 --only P1 P2 P5`, private artifacts, caches cold, on 7608a14 + the arena):
  P1 154/154 runs equal at every sync point (10,175,742 native sync points, 0 dispatch misses); P2
  passes (D2 154/154, D4 1,145/1,145, D6; the 16 changed C files verified in PC mode); P5: 0
  emu_ram symbols in the game objects (was 1,001 in 575), the exporter copies 2,086 bytes (the
  ENGINE share; was 65,536) on 154/154 runs. The gate: ok (native 6,000 equal).
  On the flow runtime (main 0775162, kit 0.10.3, engine 8cbd519), the share removed, caches cold:
  **P1 154/154 (10,175,742 native sync points, branch plans: 14 processes), P2 passes, P5 passes:
  0 emu_ram symbols in the game objects, copied_bytes 0 on 154/154 runs** (native_bytes 65,536).
  Gate 49 s: game objects cpu68 0, emu_ram 0, bus 0; link forbidden 7,449 (P6's).
- **Left for P6** (the native machine, p6-machine): the DMA source read is `dk_arena_read_be(off,
  out, n)` (big-endian, offset wrapping in 64 KB): the native VDP's DMA callback calls it, and
  platform_bus.cpp's staging into the engine's RAM goes with the bus backend; `dk_arena_init()` at
  power-on and `dk_arena_branch_save/restore()` (or a savestate section: dk_arena + dk_tags) move
  from spec.c to the native main loop. Nothing in arena.cpp or dk_mem.h's native branch references
  the engine (g_ram is referenced only by platform_bus.cpp's staging).
- **Self-test**: `DKIT_EXPORT_SELFTEST=FF816D` (the low byte of a word unit in g_ArtHandles' table,
  stored in host order): the native movie trace differs from stepped-gen from sync 402 on, in
  ram_crc32 only (984 of the first 3,000 rows: where that byte is non-zero), as at the kickoff;
  without it the first 3,000 rows equal. Same result on the final build (flow runtime, no share).

## Phase 5: the native machine (P6, rulings R3 R5 R8, 2026-10-29)

- **What it is**: `batmanrobin_native` no longer links the engine's runner. It is the native game
  library (the C, the bridge without engine_stubs.cpp, flow.cpp, hint_raster.cpp, arena.cpp,
  export.cpp, `platform/platform_native.cpp`, `platform/native/flow_native.cpp` + `game_mem.cpp`)
  plus `recomp/platform/native/` (machine.cpp, runner.cpp, input_script.cpp, host_sdl.cpp,
  native.cmake) plus, from the engine, only genesis_vdp.c, superzazu z80.c, fiber_compat.c
  (minicoro) and the sound chips/mixer (audio.c, chip_trace.c, audio/*, ymfm). spec.c, main.c,
  glue.c, exact_sched.c, ram_views.c, m68k_interp.c, the generated code and flow_host.cpp are gone
  from it (flow_host.cpp deleted; spec.c's DK_NATIVE branches removed). CMake: `game_runner(...
  NATIVE ...)` calls native.cmake's `native_runner` (the kit's `build` still finds the target by
  its `game_runner(batmanrobin_native` line); `game_handc(... PLATFORM native)` filters the bridge's
  engine_stubs.cpp (its 4,862 `func_ADDR` fatal stubs were the engine dispatch table's names).
- **The machine is the engine's frame-synchronous machine, rule for rule** (exact_sched.c's
  stepped paths with GameSpec.fsync_vint_at_wait and fsync_raster, genesis_machine.c's Z80 step,
  genesis_bus.c's Z80 bus, spec.c's wait_vint_here pads): one runner frame = the Z80's frame (V-int
  line one line from where it is, then to 262 x 3,420 master clocks, YM sample clock re-anchored),
  then the game's fiber to its next sync point (DK_SYNC_PC at interrupt depth 0: the row from the
  exporter, then it parks). IRQ_WAIT: the first V-int of an interval with the runner frame's pads,
  later ones with the VPAD queue then input_script_peek; the V-int (reg 1 bit 5, mask < 6) latched,
  taken through dk::flow::interrupt with PC 0, then the display line by line with the H-int raster
  (reg 10 counter, a latch taken at the display's start, the H-int when reg 0 bit 4 and mask < 4:
  $FFE560's RAM code in C or its jmp stubs). DMA at once (68K source through
  `dk_arena_read_be`, the I/O-area quirk kept); status 0x7200 with VBLANK alternating; H/V reads 0;
  8 Z80 lines per bus release; YM timers on the Z80's clock; the 20,000,000-read hang guard and the
  30 s watchdog. The machine's whole state is one POD block (MState) beside g_machine (the video
  model, Z80 core and RAM): a branch snapshot is the game fiber's stack + the arena's own copy
  (dk_arena_branch_save) + the flow runtime's (dk::flow::branch_save) + those two blocks. The Z80's
  EI shadow stays out of the snapshot, as the engine's static is (a branch keeps the shadow of the
  frame before the restore, exactly as stepped-gen's branches).
- **Not modelled, fatal instead** (nothing decodes instructions, R8): open-bus bits (the 68000's
  prefetch) read as 0 (the game tests only bit 0 of the bus-grant byte); a 68000 Z80-area read
  without the bus, any 68000 YM2612 access, a DMA or Z80-window access outside ROM / work RAM /
  Z80 RAM are fatal. The corpus makes none (P1).
- **Runner**: the engine's command line and outputs as the kit reads them (--benchmark,
  --input-script, --no-render, --hash-frames, --branch-plan with its #save/#end/#restore markers
  and GENESISRECOMP_BRANCH lines, [FBHASH] over a 512-stride ARGB frame as main.c's
  framebuf_active_hash, GENESISRECOMP_BENCHMARK, the counters line, DKIT_SYNC_CSV /
  DKIT_STACK_LOW / DKIT_SYNC_DUMP / DKIT_STEPPED_HANG_S). Frame pads = the script's mask & $FF
  (| $F00 on a 6-button port), as main.c's input_requested_cb. The ROM: 2 MB with the cartridge's
  SHA-1 or exit 2. settings.ini's pad_type per port.
- **Played** (no --benchmark): SDL window, keyboard + 2 controllers, the mixer drained and sent to
  the device each host frame, paced to 59.92 Hz. A further V-int inside one sync interval (a pause
  or fade loop waiting several V-ints) is its own host frame there: the fiber parks, the host
  presents, polls the pads and runs the Z80's frame, then the V-int is taken with the live pads.
  Benchmark runs never do this (their extra V-ints read the script's VPAD pads as stepped-gen's).
- **First build**: the movie's first 6,000 sync points equal stepped-gen's at the first run (no
  fix needed); `recomp scan`: link forbidden 0 (was 7,449), game objects cpu68 0 / emu_ram 0 /
  bus 0, hardware literals 0.
- **Measured** (`phase.py 5 --only P1 P2 P3 P4 P5 P6 P7`, private artifacts, caches cold, on main
  dfc1cff + this work): 7/7. P1 154/154 runs equal at every sync point (10,175,742 native sync
  points, branch plans: 14 processes; 0 stopped), P2 (D2 154/154, D4 1,145/1,145, D6), P3-P5 0,
  P6 link forbidden 0, P7 the movie's 130,055 sync points' fb_fnv1a64 equal. The four corpus passes
  at once took 571 s. Gate ok in 46 s. P9 (`recomp package --platform linux`, WSL GCC 15.2, the
  arena's first GCC build): builds, 0 ROM windows, smoke passes (exit 0; changed ROM exit 2), the
  movie's 130,055 sync rows equal stepped-gen's. Played (SDL dummy drivers): runs, no fault in 8 s.
  Headless speed: ~1,500 runner frames/s on the movie's start (stepped-gen ~1,700). Bench (kit
  0.10.4): S1 clean build of the 5 runners 112 s, S2 gate 38 s, S3 the corpus check 641 s. The
  Windows package: smoke passes (600 frames exit 0, changed ROM exit 2), its only ROM windows still
  SDL2.dll's 4 (the queued ruling); the native exe itself has none.

## Phase 6: the engine (R10, R12, 2026-10-30)

Engine pin 88b6431 (crabsmack/segagenesisrecomp `p6-vdp`, on native-flow 8cbd519). The VDP model
(`recomp/engine/runner/video/genesis_vdp.h`) gains four presentation extensions, each off or empty
by default; with none used the renderer's output and the VDP struct are unchanged. None is in the
GVDP struct (no snapshot or save image changes); none is game state.

| API | What | For |
|---|---|---|
| `int gvdp_set_ws_vscroll_column(int col, uint16_t vs_a, uint16_t vs_b)`, `void gvdp_clear_ws_vscroll_column(int col)`, `void gvdp_clear_ws_vscroll_columns(void)` | 2-cell V-scroll (VSRAM words, planes A and B) for margin column `col = floor(x/16)` in screen space: -3..-1 and 20..22 with 40-px margins (-8..27 accepted, returns 0 outside). Kept until set again or cleared, like VSRAM; an unfilled column draws the edge column's value (the old clamp). The native machine renders each line after its H-int (machine.cpp fsync_display), so a native H-int branch that rewrites VSRAM sets the margin columns at the same point | R12: $2100, $3300, $3101 |
| `int gvdp_set_host_sat(const GVDPHostSat *list, unsigned n)` (n <= `GVDP_HOST_SAT_MAX` 256), `unsigned gvdp_host_sat_count(void)` | sprites in SAT format (`y` = y+128, `size`, `link` ignored, `attr`, `x` = x+128) drawn after the SAT chain's end, in array order: the SAT's priority, S/H operators, the line's x=0 masking state, the per-line limits unless unlimited (a line the SAT filled draws none). No overflow/collision bits, no carried VDP state. Copied; kept until set again (n = 0 clears) | R10: display-list nodes past the game's 80 |
| `void gvdp_set_unlimited_sprites(int)` (existing; machine.cpp sets it from `g_opt.unlimited_sprites` at power-on) | no 20-a-line / 320-px-a-line limits; link order, priority, x=0 masking kept | R10 |
| `void gvdp_set_ws_authentic_sprite_budget(int)` (machine.cpp: 1) | with a margin or canvas the line's sprite pixel budget stays 320 (it was the output width, 400): a widened line drops what the 4:3 line drops for the same table, so E3's centre can equal the 4:3 frame. Sprites added for the margins still spend it (the hardware counts off-screen sprites): with R11's widened culls, pair widescreen with unlimited sprites or link margin-only sprites last | R4/R11 |
| `void gvdp_get_stats(GVDPStats *)`, `void gvdp_reset_stats(void)` | cumulative: `sprite_lines`, `sprite_drop_lines`, `sprite_drops_count` (on a line past the 20th), `sprite_drops_pixels` (within 20, cut or not drawn by the budget), `sprite_masked_lines`, `host_sat_sprites`, `ws_margin_columns`, `ws_edge_clamped_columns` (per line) | E3, E5 |

machine.cpp resets the counters at each drawn frame's start and adds them to `g_enh`:
`sprites.line_drops` (count + pixel drops), `sprites.line_drop_frames`, `sprites.x0_masked_lines`,
`ws.edge_clamped_columns`. It sets no group's `active` (the game's list counts and the widescreen
code complete those groups). Counted on rendered frames only (not with `--no-render`).

**Measured on the movie, every option off** (130,055 runner frames): drops on 539 frames, 234
sprite-lines past the 20th (25 frames) and 29,530 cut or skipped by the budget (539 frames); 0
masked lines. The kickoff's numbers (751 frames by count, ~5,581 by pixels) were read from the
table in VRAM, not from the renderer's walk of the link chain; not reconciled (the walk stops at
the chain's link 0, a table read may count stale entries past it). `--options
unlimited_sprites=1`: the picture differs on 416 frames, each one of the 539, and the sync rows
equal the off run's on all 130,056 rows.

**Controls.** The engine's `tests/runtime/run_vdp_render_diff.py --msvc --baseline-source <8cbd519's
genesis_vdp.c>` (MSVC support and 1500 crowded-sprite lines new: the older cases left the SAT
cache empty, so they never reached the sprite limits) matches on every case; planted changes
(the budget on by default, margin columns 0 instead of the clamp, 21 sprites a line, the pixel
limit 8 px later) each fail it. `vdp_p6_test` (SCALAR and BATCHED): each feature on, a planted
line differs; off or cleared, identical. On this tree: phase 5 P1 154/154 runs (10,175,742 native
sync points), P7 130,055 syncs, the native movie trace byte-identical to 8cbd519's, the per-batch
gate ok in 43.6 s. The engine's `vdp_sprite_limit_test` / `vdp_host_sprite_test` had failed since
the SAT cache (engine 0352aff: they wrote VRAM past it); fixed on the branch.

**Widescreen audit** (engine WIDESCREEN_ISSUES.md "The renderer at 16:9"): per-line H-scroll,
per-line full V-scroll (the H-int raster), plane wrap (512/1024-px planes), 10-bit sprite x
(screen -40..359 = raw 88..487, inside 9 bits), x=0 masking and the collision border already work
in 40-px margins; the host (host_sdl.cpp) presents any width up to 512 (texture 512x480, aspect
kept). Not handled, not needed here: the window plane in the margins, 32-cell planes, square
pixels (400x224 = 1.79:1). What the native side still calls: `gvdp_set_ws_extra(40)` (the margin)
and the per-scene `gvdp_set_ws_vscroll_column` fills (widescreen worker); `gvdp_set_host_sat` from
the raised display list (the sprite worker); `sprites.active` once the list counts are in.

## Phase 6: the contract (kit-p6, 2026-10-30)

What `tools/phase.py 6` (kit `dkit.phases.p6_enhance`, ruling R7) drives, and the hooks the
option work fills. Every check against the original runs with every option at its cartridge
setting, read from the runner's own `--options --json` (`cartridge`): the kit now passes that to
every native run that names no options (phase 5's checks included), in the plain
`artifacts/recomp/native/` dir; any other options or diagnostics land in `native-o<hash>/`.

- **Options** (`recomp/platform/native/options.{h,cpp}`, record `g_opt`): `widescreen` 0|16:9|16:9-gameplay
  (R23, 2026-11-17: one setting; it was `widescreen` 0|16:9 plus `widescreen_gameplay` 0|1), `high_fps` 0|120|144|165|240 (any 61-1000), `unlimited_sprites` 0|1,
  `music_voices` 1|0 (default 1, R13; cartridge 0), `fm_ladder` 1|0 (1 = the cartridge's ladder),
  `fm_filter` 0|1, `weapon_level` 0-6 (R24: Main_Start's g_StartWeaponLvl through dk_opts.h). Set by `--options K=V,...` on any run and by `settings.ini` `[options]` beside
  the exe (the command line wins); `--options --json` lists default, values, cartridge, value,
  what, and the effective values (no ROM needed; `dkit genesis recomp options` checks the settings
  file read back). Played runs: F9 high_fps off/on (144 if none given), F10 widescreen 0 -> 16:9 ->
  16:9-gameplay -> 0 (R26); each shows its new setting on the picture for 2 s (host_osd).
- **Runner outputs** (`runner.cpp`, `--benchmark` runs): `--present-log F`
  (`sync,t_us,whole,gameplay,fb_fnv1a64`: runner frame k's picture at k x 16,688.2 us, one NTSC
  frame of 262 x 3420 master clocks; with high_fps the clock's ticks strictly between (k-1)T and kT
  are interpolated toward R_k, or R_(k-1) again with no interpolator), `--flip N` (player 1's C
  inverted in runner frames N..N+7; the VPAD pads of extra V-ints are not flipped), `--game-rows`
  (`game_crc32,gameplay` beside each sync row; the kit writes OUT/game.csv), `--hash-centre`
  (`fb_centre`: FNV-1a-64 of the centre 320 columns, `g_enh_centre_x0/_skip` for the x 0-15
  cover), `--render-from F`, `--audio-hash`, `--suppress-sfx`. With none of them the sync rows are
  written exactly as in phase 5 (the extra columns hold the row to its runner frame's end). At exit
  `[dkit] options {...}` and `[dkit] enhance {...}`; each branch's counters in its
  GENESISRECOMP_BRANCH line (a branch counts from zero, the carrier's counts are put back).
  Checked on the movie's first 3,000 sync points: the columns leave the sync rows and --fb hashes
  equal stepped-gen's; fb_centre == fb_fnv1a64 in 4:3; a picture from `--render-from` equals the
  always-drawn one (the render is stateless); a flip at 1000 changes ram_crc32 from sync row 1000.
- **Gameplay frame** (`enh_gameplay`): g_SectionId ($FFF602) in $1100-$4FFF (a stage section, not
  an opening $0001-$0004 or 0) and g_FadeLevel ($FFF703) = g_FadeTarget ($FFF702) = 0. The movie
  is in gameplay from sync 420 (2,581 of its first 3,000 sync points).
- **The game's state** (`game_crc32`): work RAM with the sync rows' masks and view_masks.h's RAM
  rows zeroed; VRAM less the plane A, plane B and window name tables and the sprite table (from
  the VDP's registers at the sync point) and view_masks.h's VRAM rows; CRAM; VSRAM. view_masks.h
  starts empty: each worker adds the ranges its option changes, with every reader.
- **Counters to fill** (`enhance.h`, plain structs; set `active` = 1 or the group is null and its
  criterion fails naming the piece): `g_enh.ws` (gameplay_frames, terrain_checks, terrain_errors,
  sprites_lost, hud_misplaced, bars_unneeded, edge_clamped_columns, centre_mismatch,
  `enh_ws_screen(id, mode)`), `g_enh.sprites` (line_drops, line_drop_frames, list_drops,
  list_drop_frames, x0_masked_lines, cover_frames; filled with the option off too: the reference's
  drops), `g_enh.audio` (keyons_by_channel[6] machine 1, music_machine + music_keyons_by_channel[6],
  notes_lost, sfx_sent, sfx_started, machine2_reads, fade_pause_mismatch, audible_outside_effect,
  pcm_hashed/pcm_fnv1a64/pcm_samples), `g_enh.present` (sprites, sprites_matched,
  `enh_scroll_frame(section, interpolated)`). Hooks: `enh_set_interpolator(fn)` (high_fps),
  `g_enh_audio_frame(frame)` (the sound side, each headless runner frame before the event queue is
  emptied), `g_enh_flags` (the diagnostics asked for).
- **How each criterion is measured** (kit 0.11.0): E1 phase 5's P1-P7 and P9 (its result cached by
  content: runners, sources, inputs, kit); E2 the listing, the settings file, and the movie with
  `--fb` every option explicit vs `music_voices=0` only; E3-E7 first a 3,000-frame movie probe
  with the option: no counters from the piece fails at once; then each option's corpus pass (8
  runners) against the option-off pass with `--game-rows --hash-centre` (E3, E5), stepped-gen (E4),
  the `--suppress-sfx` reference (E6); E4's movie at 144 and off with `--present-log --fb` and 10
  latency probes (`--flip N --render-from N-2`, the first gameplay sync point from each spread
  point whose flip changes the picture within 40 frames); E8 both packages list the options.
  Stage times in artifacts/p6-stages.json (S3).
- **First measured** (2026-10-30, shared artifacts dir, kit 0.11.0): E1 cold 1,247 s (the four
  phase-5 corpus passes at once, 4 runners each: 664 s; the picture traces 197 s; the Linux
  package in WSL 218 s), warm 356 s (traces cached), from its cache 3 s. E2 passes (the
  music_voices=0 movie with --fb 181 s). E3-E7 each fail at their 3,000-frame probe (0.1-10 s)
  naming the missing piece; E8 both packages ROM-free and listing the options (Windows 9-19 s,
  Linux from the shared cache), failing only on `enhancements-verified`. Bench: clean build 102 s,
  gate 46 s with the cartridge options. The whole draft run: 606 s warm, 26 s with E1 cached. The
  corpus, branched: 779,716 runner frames in 9 processes a pass; with the picture (~1.1 ms a
  frame) a pass is ~15 min of CPU, its time set by the longest process.

## Phase 6: music_voices (R13, R14) and the FM options (R15), 2026-10-30

- **The driver, as it matters here** (Z80 listing of Snd_1E6104, the session scratchpad's z80.lst):
  six channel records at $1480+$67n, record n = FM n+1. A record's +$00..+$49 is its music voice,
  +$4A.. its effect voice. The driver alternates two halves a tick ($0095): records 0-3's output
  ($0B52) and the effect intake ($16F5, $17A0); records 4-5's output and the sequencer ($0317/$01FE
  for records with +$1F on). **The effect window is $17EA-$180D**: six 6-byte slots (the kept
  priority, the hold, the data pointer), slot n for record n; Snd_SendSfxBody writes the pending ones
  every full V-int (`dbra d7 #5`), the Z80 takes a slot whose first byte is non-zero ($17B9: record
  +$00 = $FF, +$01 = the hold; the slot's first byte cleared). The effects use slots 3-5 only
  (`Tbl_1E50D6`: +$12, +$18, +$1E), i.e. $17FC-$180D: the kickoff's "$17EA-$17FF" (and its replay
  harness's music-only mode) covered slot 3's first 4 bytes and none of slots 4-5 (70 of the 75
  effects); the pause byte $180E and the state byte $180F follow the window.
- **The one key-on**: every note is `$28 = channel | $F0` at $0CD0. `$0B52` reaches it with IY = the
  record (the music's note: the note flag +$3F set by the sequencer, taken at $0BAF with
  `LD (IY+$3F),0` at $0BB6) or IY = record + $4A (an effect: record +$00 non-zero, cleared at $0B7B,
  the effect voice keyed). While the hold +$01 counts down, the record's output is the effect
  voice and the music's note flag is not taken: a second note set before it is taken replaces the
  first (lost), so only the last pending note is keyed at the hold's end. After an effect the music
  reloads its instrument at its next note (key-off, instrument, key-on: +$1D/+$1E were cleared),
  so on the cartridge an effect already rings until the music's next note (R14's ring-out).
- **Machine 2** (recomp/platform/native/sound2.cpp, with music_voices): machine.cpp's Z80 side rule
  for rule on a state of its own (the superzazu core, 8 KB RAM, bank register, the YM2612 timers;
  z80_frame, the 8-line slice at a bus release, the reset and bus-request lines), started as a copy
  of machine 1 (power-on, or when the option turns on), fed every 68000 write to Z80 RAM except the
  window and every line change. Its bank window reads ROM / Z80 RAM / work RAM; any write it makes
  outside its own RAM, the PSG, a read the bus doesn't decode are dropped and counted (0 on the
  movie). The 68000 reads only machine 1; machine 2 is in the branch snapshot (sound2.cpp
  `snd_save`), not in any sync row.
- **Machine 1's synthesis with music_voices**: a key-on with IY at a record (the music's) goes to
  machine 1's chip as a key-off (`$28 = channel`), an effect's key-on as it is: a channel sounds
  from an effect's key-on until the music's next key-on (the driver's instrument-reload key-off
  comes first, as on the cartridge) or until the envelope is silent (R14). Everything else goes to
  the chip as written. The machine itself (RAM, registers, timers) is untouched: only the copy of
  the write the synthesis gets changes.
- **The synthesis** (sound2.cpp FmSynth): with music_voices=0, fm_ladder=1, fm_filter=0 machine 1's
  chip writes go to the engine's event queue and mixer exactly as before. Otherwise each machine's
  writes go to a ymfm chip of its own, drained as mixer.c does it (the address/data pair guard, the
  stable stamp sort, writes past the frame carried, a trailing address carried) and rendered as
  ym2612_ymfm.cpp does (gain 195/256, the 6.910/4.910 output low-pass). One `ymfm::ym3438` object a
  machine: `ym2612::generate` (the ladder's DAC discontinuity, the cartridge) or `ym3438::generate`
  (ladder-free: fm_ladder=0, as the engine's fm_shadow.cpp with GENESIS_FM_LADDER=off); fm_filter=1
  is fm_shadow.cpp's relaxed low-pass (0.90/0.10). The mix is machine 1 + machine 2 less one silent
  chip's output (the ladder's DC offset: +4 a silent channel), so a frame with no effect is machine
  2's sound exactly. The PSG stays the engine's (the game writes it 4 times at boot: silent).
- **Headless**: `--audio-hash` (enhance.h) renders and hashes the mix (FNV-1a 64 into
  `[dkit] enhance`'s audio, CRC-32 of each stem in `[dkit] sound`); `BR_WAV=PREFIX` writes
  PREFIX.mix/.music/.sfx.wav (53,267 Hz stereo FM), headless with `--audio-hash` or played.
  `--suppress-sfx` (E6's reference) makes machine 1's Z80 read the window as 0: dropping the
  68000's window writes instead would hang it (Snd_SendSfxBody repeats each byte until it reads
  back). The runner empties the event queue after the sound hook; a headless render through the
  engine's mixer keeps the writes the mixer carried past the frame and puts them back at the next
  frame's start, as a played run does (without that the movie's PCM parted from the played path's).
- **Counters**: `[dkit] enhance`'s audio (kit-p6's contract) and sound2.cpp's own `[dkit] sound`
  (per machine: key-ons per channel by whose, notes set / keyed / lost; effects sent / taken /
  started and each way the driver drops one; machine-2 reads, game writes, PSG writes; fade/pause
  mismatches; the stems' CRCs). Notes lost are counted on machine 1 from its note flags (a pending
  +$3F replaced, or cleared anywhere but $0BB6), so the count works with the option off.
  `tools/soundcheck.py e6|e7|control INPUT...` runs and compares them.
- **Measured** (2026-10-30, `tools/soundcheck.py`, the trunk inputs as `recomp stepped` writes them):
  - The movie (130,055 sync points): the sync rows with music_voices 0 and 1 equal each other and
    the native trace from before sound2.cpp, z80_crc32 included. Machine 2's key-ons per channel
    [10470, 9737, 9070, 7794, 8845, 7161] = the reference (`--suppress-sfx`): 0 notes lost; with it
    off **176 notes lost** (FM4 1, FM5 31, FM6 144), equal to machine 2's key-ons less machine 1's
    music key-ons. Effects: 830 sent, 830 started (on and off). Machine-2 reads 0; machine 2's game
    writes, PSG writes, effects started 0; fade/pause mismatches 0 of 130,055 frames; machine 1's
    chip got 0 music key-ons (52,901 sent as key-offs). The music stem equals the reference's sound
    sample for sample (CRC-32 a9b7281c over the movie). By the song playing (g_MusicCur) and
    g_SectionId when it happened, [notes lost, effects sent]: song 1 / $1100 [116, 485], song 8 /
    $1100 [59, 345], song $13 / $0010 [1, 0]. The movie's effects all come in runner frames 0-~12,500
    and ~85,000-~92,500; for the rest g_MusicCur is $11 and g_SectionId $0001-$0003 and the game
    sends no effect (not looked into: what the movie is doing there).
  - `tools/phase.py 5 --only P1 P7` with the runner's defaults (music_voices=1): P1 154/154
    (10,175,742 native sync points), P7 130,055 equal; the gate ok (52 s).
  - The kit's draft (kit 0.11.0): E7 passes (the movie's PCM off / fm_ladder=0 / fm_filter=1
    5886480928B86B97 / DBC94612ABAE7F43 / 9879D484DAC561B6, sync rows equal). E6 over the corpus
    (branched, 154 runs; after the rebase onto the sprite work: the music pass 230 s, the reference
    188 s, the off pass 198 s; 627 music notes lost with it off over the corpus, by the contract's
    key-ons): sync rows on == off in every
    run, machine 2's key-ons == the suppressed reference's in every run, machine-2 reads, fade/pause
    mismatches, audible-outside-effect 0; it fails only on "effects sent but not started" in 4 runs
    (fuzz-05400-s1 2 of 220, fuzz-07200-s1 2 of 143, fuzz-09000-s1 9 of 142, ...): each one an
    effect the game wrote over in its slot before the driver read it. The driver takes the slots
    every other tick (~once a frame), so the cartridge drops these too (not a frame-synchronous
    artefact: 0 of them at a sync interval's second V-int). Queued in QUESTIONS.md (default: not
    counted as sent; the contract's `sfx_overwritten` reports them).
  - The contract's `keyons_by_channel` (machine 1) counts the music's key-ons only (IY at a record),
    so the suppressed reference's minus the option-off run's is the notes the effects took.
  - E7 on the movie: the synthesis here at the cartridge's settings equals the engine's mixer
    sample for sample (d567e2be); fm_ladder=0 (8b1ff09c) and fm_filter=1 (f78bef4e) change the PCM,
    the sync rows equal.
  - A corpus sample (trunk, no branches): scenario-two-player 3 notes lost with it off,
    scenario-game-over 153, scenario-level-skip 46 (227 effects started of 230 sent: 3 overwritten
    in their slot before the driver took them, the cartridge's own drop, the same on both),
    fuzz-54000-s1 153, fuzz-111600-s2 176, scenario-ending 176: every e6 check passes. In
    level-skip machine 2 is one note ahead on FM2 and FM3 at the end (its ticks are shorter without
    effects), so the key-on difference there is 1 more than the notes lost.
  - Negative control (`BR_SND_PLANT=1000`: the first 68000 read from the 1,000th on where the
    machines differ comes from machine 2): `machine2_reads` 1, e6 fails. The sync rows stayed equal
    (the read was Snd_SendSfxBody's read-back, whose loop wrote the byte again: one effect sent
    twice), so the counter, not the rows, is what catches a feedback.
  - **Effects in flight at a run's end** (music-fix, 2026-10-30; E6 failed on the 171-run corpus,
    "sent (less overwritten) but not started: 2 runs"): not a drop. scenario-sec-3200-s1 ($3200,
    10,708 frames) ends with effect 11 ($0B, Tbl_1E50D6 slot 5, data $1E551A) in the window, sent
    at the run's last V-int (after the last frame's sound ran); scenario-sec-4401-freeze (14,086)
    with effect 60 ($3C, slot 5, $1E5AD8; the Freeze fight sends it every frame) twice: the one
    sent at the last V-int in the window, the previous frame's taken (record +$00 = $FF) and not
    started. Identical with music_voices=0 (machine 1 is the cartridge's driver either way: the
    counters are its). The pipeline, measured by running the input longer: a send at frame N's V-int
    is taken during frame N+1 and started during N+2 (10,709 frames: 2 in flight; 10,710: 1; 10,768:
    175 of 175 started; the Freeze run +60: 3,079 started, 1 in flight). The old 154-run corpus
    had no run ending within a frame of a send. sound2.cpp now stamps each slot's send and take with
    a frame counter in the branch snapshot (FxStamps) and reports, in the contract, `sfx_in_flight`
    and `sfx_in_flight_age` (computed at the run's or the branch's end) and `sfx_start_latency_max`
    (the slowest send-to-start of the run's started effects: 3 frames here); the `[dkit] sound` line
    lists each one (`sfx_in_flight`: slot, window/taken, data, sent frame). Kit 0.11.6's E6: started
    == sent - overwritten - in flight, with an in-flight effect older than the slowest start counted
    as stuck (fails). Control `tools/soundcheck.py control-stuck` (BR_SND_PLANT_DEAF=10000: machine
    1's Z80 reads the window as 0 from frame 10,000, a driver that takes nothing more; sec-3200-s1 at
    10,600 frames): sent 169, started 142, overwritten 26, in flight 1, so the subtraction alone
    balances (142 = 169 - 26 - 1, the contract line's check passes); the in-flight age 11 > 3 FAILS,
    as do the rows on == off. Limit: an effect in flight at a branch's START and started inside it
    counts as started without being sent in the branch: it fails E6 (safe, not hidden); no corpus
    branch does it. Queued in QUESTIONS.md (default: count them apart).
  - Stems, the movie's first 20,000 frames: mix = music + effects - 380 (one chip's ladder offset)
    exactly, nothing clipped; AC rms 2,947 on / 2,918 off; the effects stem sounds in 6,187 of
    20,015 frames (`artifacts/wt/music-p6/wav/`, not committed).
  - Cost: the headless movie, best of 2: 0.110 ms a frame off, 0.199 on (14.2 s -> 25.8 s): the
    second Z80 ~0.09 ms a frame (played: 0.5% of a frame).

## Phase 6: unlimited sprites (E5, R10, sprites-p6, 2026-10-30)

`unlimited_sprites=1` lifts the VDP's per-line limits (the engine, machine.cpp) and the game's two
80 caps: the display list's 80 nodes (`g_SprNodesFree`, Spr_Add*) and the sprite table's 80
entries (Spr_BuildSAT's `cmpi.b #$50`). Native branches only (R2: `#if DK_MODE_NATIVE`; the
emulated modes compile none of it and verify keeps proving the C), host side in
`recomp/platform/native/spr_host.{h,cpp}`.

- **Refused nodes stay off work RAM.** In each Spr_Add*'s "none left" branch (007A4A, 007B50,
  007B1C) `spr_host_list_refused` records what the add was given (frame, x, y, attr, the key) and
  the game takes the refusal's path unchanged: its list, `g_SprNodesFree`, every register and flag
  handed back are the off run's. Refusals only happen once all 80 are used, so every refused node
  comes after every listed one.
- **The merged walk** (007B72, after the original walk, which runs unchanged): the list walked
  again as the original does (the front entries, then the sorted list from `g_SprSortFirst` to its
  `$FFFF` end slot), the kept nodes merged where an uncapped list holds them: front ones after the
  listed front entries; sorted ones (key k > 0) before the first key node whose key is below k
  (the walk breaks at the first key <= k, an equal key chains after its last entry), among
  themselves by key descending (unsigned) then order; back ones and key-0 sorted ones (the tail
  sentinel's chain) last. Each node's pieces are made as `emit_pieces` makes them (same clip
  SAT_MAX_Y/SAT_MAX_X/SAT_BIAS, flips, tile, attribute; a node whose art is not resident is
  skipped). With the option on: the table's list entries (as many as the original wrote) are
  rewritten in that order, link bytes, `g_SprCount`, `g_SATPtr` and the registers kept; the rest go
  to `gvdp_set_host_sat` (drawn after the chain's end, so behind every table sprite, in order).
  **Self-check on every frame with no kept node** (both settings): the walk's first pieces equal
  the original's entries field for field (link bytes included), else `dk::fatal`. It held on every
  frame of the movie and the corpus, which proves the walk against the C.
- **Timing of the host table.** Spr_BuildSAT leaves it pending; `Spr_QueueSATDMA` commits it when it
  queues the table's DMA (a full queue keeps the shown one, as VRAM keeps its table);
  `VDP_ClearSAT` (Scene_InitCommon) clears it. Branch snapshots carry the host state
  (machine.cpp): the sync point ($9BF0, Frame_End's entry) falls between a frame's adds and its
  table, so the refused nodes of the frame in flight are in the snapshot.
- **Counters** (`g_enh.sprites`, `active` from power-on): `list_drops` = nodes the list refused
  plus nodes the table's 80 cut (wholly or partly), counted with each frame's table (or at the
  next list reset); with the option on only nodes past the host's 256 kept nodes or 256 host
  sprites (GVDP_HOST_SAT_MAX) count. `cover_frames` = frames Spr_MaskLeftColumn drew the cover.
- **The game's state with it on** (`--game-rows`): view_masks.h masks `g_SATBuffer`
  ($FF9B56-$FF9DD5), written by the table's writers and read by no game code (only
  Spr_QueueSATDMA's DMA to the VDP's table, which game_crc32 already leaves out); the list nodes,
  the counts and `g_SATPtr` are not masked and equal.
- **Art and DMA budget: unchanged by construction.** Every draw op calls `Spr_ChangeFrame` (the art
  cache, `Spr_QueueArtDMA`, the DMA budget) before its Spr_Add*, and nothing after reads whether
  the add took the node (the refusal's outputs are the off run's): a node past 80 already gets its
  art today, so raising the cap changes no art state and needs no budget of its own. game_crc32
  (VRAM tiles and the art cache in RAM included) equal at every sync point confirms it.
- **Measured** (`tools/phase.py 6 --draft --only E5`, 59144d2, kit 0.11.0: **E5 passes**, 154 runs,
  1,105,605 runner frames a pass, 1,033 s cold). Off: 266,577 line drops on 4,546 frames, 8,400
  list drops on 2,205 frames (the movie 29,764 / 539 and 4,072 / 1,077; fuzz 192,019 / 2,930 and
  4,318 / 1,122, of which fuzz-10800-s2 2,013 / 561, fuzz-91800-s2 1,938 / 481, fuzz-90000-s1
  300 / 62, fuzz-91800-s1 67 / 18; scenarios 44,794 / 1,077 and 10 / 6, scenario-two-player). On:
  0 / 0 on every run; cover_frames 796 and x0_masked_lines 0 on both. game_crc32 equal on all
  10,175,742 sync rows, and so is ram_crc32 (no kept node ever sorted ahead of a table entry in the
  corpus: the table in RAM never changed, the mask is for the case where one does). Pictures
  (fb_centre) differ on 1,090 movie rows and 9,239 rows of the 15 runs that differ past their
  movie prefix, each a frame whose off run dropped something (the off runs' per-frame drops from a
  temporary per-frame dump, solo). The kickoff's BizHawk drop runs (cov13-hat2p*, the game-over
  scenario from stage play, fuzz-111600-s2 in stage 3) do not drop natively: the native corpus has
  no cov13 runs, and its scenario and fuzz runs start from the native movie's states (below).
  Negative controls (each fails E5): the host keeping 1 node a frame; a write outside the masks
  when a node is kept.
- **The movie's drop counts, reconciled.** Engine-p6's 539 frames (234 by count on 25 frames,
  29,530 by pixels) and the kickoff's 751 by count / ~5,581 by pixels are both right for the run
  each measured: the kickoff read VRAM in BizHawk's replay of the TAS (every stage); the native
  runner plays the movie on the frame-synchronous machine, where it desyncs in stage 1 and never
  leaves it (above, "Seen by the stepped machine"): 15,141 of its 130,055 runner frames are in
  section $1100; the rest are the openings and attract scenes ($0001 89,742, $0005 8,350, $0002-4,
  $0010) and the game-over screen ($0011, 2,060 frames between f11051 and f92234). The kickoff's own
  formula (measure.py) applied to the native runner's table at each drawn frame gives exactly the
  engine's frames: 539, of them 25 with a line past 20 sprites (234 sprite-lines). The
  stale-entries guess does not hold: measure.py follows the link chain from entry 0 and stops at
  link 0 / 80 / a link >= 80, the renderer's walk; the VDP's internal SAT cache equals VRAM there.
  The same desync puts the native movie on the game-over screen, whose snow overfills the list:
  4,072 refused nodes on 1,077 of its 2,060 frames (the kickoff saw none on BizHawk's movie); with
  the option on the missing flakes are drawn (f92151: 11 host sprites, 129 pixels differ).
- **For the widescreen and high-fps workers.** A widened clip must change SAT_MAX_X for both walks
  (`emit_pieces` and `walk_node`); with the option off the self-check fatals on the first frame
  where they disagree. Margin draws (R11) add nodes: the host keeps 256 refused nodes and 256 host
  sprites a frame (more counts as list drops). Host sprites are behind every table sprite, so the
  x 0-15 cover still hides them. high_fps's owner tags (948b1d5) are in the original walk only:
  host-table sprites and kept nodes carry none (a frame whose table was rewritten fails
  present.cpp's table check and stays unmatched); tagging them belongs in `walk_node`, which sees
  every piece in drawn order, table and host (kept nodes would need hf_spr_add's owner recorded in
  spr_host_list_refused).

## Phase 6: widescreen (ws-p6, E3, 2026-10-30)

`recomp/platform/native/widescreen.{h,cpp}` (the model: X-Men 2's platform/native/widescreen.cpp),
hooked by machine.cpp around each drawn frame (`ws_frame_setup` after the V-int, `ws_line` before
each line is drawn, after its H-int; `ws_frame_check` after the display) and by the game's native
branches through `recomp/game/include/dk_ws.h` (0 / no-ops in the PC modes: the off path is the C
verify proves). Nothing it does at render time touches work RAM: the presentation mode changes VRAM
only while a frame is drawn, or the name tables (masked from `game_crc32` by the contract).

- **Screens (R5)**: a stage section ($1100-$4FFF) by g_SectionId, a plain scene by the header
  Scene_InitCommon read (`WS_SCENE_BEGIN`, kept in the branch snapshot: `ws_saved_*`). Every stage
  section and the game-over skyline are wide; every other plain scene is 4:3 with bars (the canvas
  stays 400 px). Each with its reason in `recomp/game/widescreen_screens.toml`, checked by rendering
  every corpus screen with margins (`BR_WS_ALLWIDE=1 BR_WS_SHOT=dir`): their pictures are 320 px.
- **The streamer, widened**: the game streams g_LayerA's block columns rel 0..21 from block_col
  (22 x 16 px). Before each drawn frame widescreen.cpp writes rel -5..-1 and 22..26 into the plane's
  name table, walking the map as Layer_QueueColumn does (the block row, a map row down per block,
  the rewind at the map's height, the plane's row wrap); the game's queue, RAM and timing stay the
  original's. Not while the a6 redraw (VM6_StartRedraw: g_A6ResumeHook = $743A) walks the layer's
  columns. Layer sections: 1300, 1301, 1400, 2300, 2400, 3100, 3200 (plane A), 2100 (plane B).
- **The render camera (R16)**: on the streamed map, the 400-px window moves only where it would
  leave the map ([0, width x 16) in layer x): shift in -40..40, applied to every H-scroll word and
  every non-HUD sprite while the frame is drawn (X-Men 2's shift_vram), a VDP-x-0 sprite left alone
  (the masking rule). A map narrower than 400 is centred with bars past its ends, except on 2-cell column V-scroll frames (shift 0: the lift, 304 px, below).
  Sections drawn whole (no streamed layer: 1100, 2200, 2401, 3101, 3300, 3400, 3401, 4000, 4300,
  4400, 4401) keep shift 0 and get bars where the picture ends: a margin pixel showing nothing (both
  planes the clear tile $05FC) on a line whose 4:3 edge pixel shows something bars its column and
  every column outward (3101's 320-px arena: 40 px each side). The 4:3 view's x 0 is at frame x
  40 - shift: `g_enh_centre_x0`; `g_ws_view` (widescreen.h) carries margin, shift, bars and
  centre_x0 for the present path (high_fps).
- **The HUD**: the players' panels (HUD_DrawPanelSprites; their tile words from Tbl_011974, player 1
  left, and Tbl_0119AC, player 2 right) are moved x - 40 / x + 40 while a frame is drawn; the x 0-15
  cover (Spr_MaskLeftColumn, tile $5D8) to the wide left edge.
- **The boss meter and the GO! sign, pinned (ws-tail, 2026-10-30)**: the two HUD items the game
  writes straight into its SAT buffer. Their writers tag the entries they wrote (native branches:
  `BossMeter_DrawSprite` 011C5E.c, `HUD_UpdateGoSign` 011A1C.c -> `Spr_DrawDirect` 007D5E.c;
  `ws_hud_tag` in ws_draw.cpp, the tags following the table as the margin pieces do: shown at
  Spr_QueueSATDMA, none after VDP_ClearSAT, reset with the buffer at Spr_ResetList, in branch
  snapshots), so the frame drawn from that table knows its VRAM entries; kept in every native run,
  the 4:3 run's centre exceptions read them too. Where each is pinned (`hud_move`):
  - **The boss meter** sits between the panels (its x = the mean of the panels' home x + $A2,
    BossMeter_Init), so with the panels at the edges it stays at the screen's centre: **pinned =
    not moved by the render camera** (every other sprite moves -shift).
  - **The GO! sign** slides: it starts at screen x 360 (g_GoSignX $168, HUD_StartGoSign: one margin
    past the 4:3 edge), brakes to a stop ~79 px inside the right edge (-4.0 px a frame, +$1A00 a
    frame), waits for the camera to pass g_GoSignCameraX, then leaves the way it came. Its path is
    the right edge's, so **pinned = to the right edge, x + 40**, as player 2's panel: it enters
    through the wide view's edge on the frame the 4:3 sign enters through the 4:3 edge, rests as far
    from the edge as in 4:3, and leaves through it. Not chosen: leaving it with the render camera
    (ws-r11's margin pieces showed it sliding in from the wide edge at once and resting 119 px from
    it, moving with the level whenever the render camera shifts). Its pieces Spr_EmitClipped drops
    (x >= 320) are then past the wide edge (x >= 360): `ws_direct_piece` (007D76.c) draws no margin
    piece for a tagged direct draw.
  - **The checks**: `hud_misplaced` counts a frame where any panel piece, the meter or a sign piece
    is not at the game's x moved by its `hud_move` (check_sprites: each item recorded moved or not);
    the ws JSON's `hud_frames` (panel_p1, panel_p2, boss_meter, go_sign) counts the wide gameplay
    frames each item was checked on; their rectangles join `g_enh_centre_rects` (the default in
    force, QUESTIONS.md): where the 4:3 view draws them and where the wide view pins them (one
    rectangle where the two coincide), ENH_CENTRE_RECTS 32 -> 48 (`centre_rects_full` counts a piece
    past it). Control: `BR_WS_PLANT_HUD=n` with `BR_WS_PLANT_HUD_KIND=k` (1 player 1's panel, the
    default; 2 player 2's; 4 the boss meter; 5 the GO! sign) pins that item n px off.
  - **Measured** (private corpus passes, 154 runs, --game-rows --hash-centre): widescreen=16:9 vs
    4:3: game_crc32 equal at every sync point, fb_centre equal on all 1,723,283 gameplay rows; 0
    hud_misplaced, terrain errors, sprites lost, edge-clamped columns, centre_rects_full; the meter
    checked on 32,166 wide gameplay frames (9 runs: tas, scenario-two-player-exit, fuzz-05400/07200/
    09000...), the sign on 1,855 (4 runs: fuzz-90000-s2, tas, fuzz-03600-s2, fuzz-88200-s2). The
    gameplay mode: all 154 runs to their end (10,175,742 sync points, no stop), 0 misplaced. Controls
    (tas, fuzz-90000-s2, fuzz-09000-s1): `BR_WS_PLANT_HUD=8 BR_WS_PLANT_HUD_KIND=4` hud_misplaced
    9,434 = every meter frame; `KIND=5` 1,638 = every sign frame.
- **R12 columns**: with 2-cell V-scroll (reg 11 bit 2) the margin columns -3..-1 and 20..22 take
  the line through columns 0 and 19 (per line when the raster rewrites VSRAM): the lift's symmetric
  bands (the edge value carries on), the spotted room's shear (16x the wobble a column), the cat
  head's sway ramp (its bulge covers columns 1-18 only), uniform H-int rewrites (the lift's floor).
  Rounded to the pixel; the game's 16.16 ramps round the same way within 1 px.
- **No render camera on column-scroll frames (ws-centre, 2026-10-30)**: the lift's map ($2100,
  plane B streamed) is 19 blocks (304 px) at layer x -16 on every frame (map at screen x 16..320, the
  x 0-15 cover over the rest), so shift_for centred it in the 400 px with a render camera shift of
  +8. The shift moves the planes through the H-scroll words only; the VDP's 2-cell V-scroll columns
  stay on the screen's 16-px grid (the engine's column of output x is (x - margin) >> 4), so every
  column's scroll was drawn 8 px off its own pixels. While no column edge of the lift shows it (all of
  scenario-level-skip's visit, idle) nothing shows; once its towers scroll apart,
  the first 8 px of each 4:3 column show the next-left column's scroll (the towers torn at every
  column edge). Found by corpus-p6's section runs: E3 (kit-combo, 9f07c18, 171 runs) failed with
  fb_centre unequal on 2,792 gameplay rows in each of scenario-sec-2100-s1 / -s2 (from their sync
  3736 to the end; game_crc32 equal on every row); played alone, sec-2100-s1 sync 4303 (the first):
  18 pixels on line 0 at centre x 98-214; sync 5000: 1,542 pixels on 136 lines, every one in the
  first 8 px of a column (x 96-103, 128-135, 192-199, 208-215, 240-247), the same pattern on lines
  0-110. Fix: `if (p.vcol) s = 0` in shift_for, so the wide view keeps the 4:3 view's columns (bars
  40 | 40, the map at frame x 56..360; the 16 px left of it the cover hid in 4:3 show, as at +8).
  Played alone after it: fb_centre equal on all 7,289 rows, 0 terrain errors / edge-clamped
  columns / HUD misplaced; the gameplay mode to its end, 0 errors. Not chosen: an engine column
  origin (gvdp: the V-scroll column of output x taken from x + shift, the margin columns widened to
  -5..24) to keep the map centred: an engine change for 8 px of centring. **Seen, not changed**: the
  bars are written over the finished frame, so in the lift the pinned HUD (player 1's panel at frame
  x 17..123, pinned to the wide edge) is under the left bar: 23 px of it hidden (15 px with the old
  +8 and its 32-px bar; player 2's side 40, was 48). By the same geometry the Cat arena $3101 (bars
  40 | 40) has it too (not looked at). (Fixed by ws-hudbars: the next item.) Diagnostic: `BR_CENTRE_SHOT=DIR` with
  `BR_CENTRE_SHOT_SYNC=a,b,...` (enhance.cpp) writes each listed sync row's frame as a PPM, with the
  centre's x0, skip and rectangles beside it: what `--hash-centre` hashed.
- **The pinned HUD over the bars (ws-hudbars, 2026-10-30)**: the bars (shift_for past a streamed
  map's ends, content_bars past a picture drawn whole) were painted over the finished frame, so
  wherever a screen has bars the HUD pinned to the frame's edges was partly under them, and E3's
  hud_misplaced (each piece at the game's x -/+ 40) passed.
  - **Measured** (16:9 corpus pass with the old bars, 171 runs; the gameplay mode the same to a
    few pixels): only the players' panels, in five sections; the boss meter (centre) and the GO!
    sign (never on a barred screen in the corpus) never. Per section, the frames (in
    scenario-level-skip; the section runs branched from it repeat its prefix) and how far the
    piece reaches into the bar (its outermost opaque pixel from the bar's inner edge): the lift
    $2100 (bars 40 | 40) player 1 257 frames, 36 px (the portrait), player 2 237 frames, 18 px
    ("GAME OVER" / "PRESS START"), and 3,158 frames in each of scenario-sec-2100-s1/-s2; the Cat
    arena $3101 (40 | 40) player 1 670 frames, 40 px (the portrait wholly hidden), player 2 454,
    18 px; $3300 player 1 225 frames, 22 px; $3401 player 1 660 frames, 40 px, player 2 59, 12 px;
    $4400 (content_bars, a narrow left bar) player 1 307 frames, 35 px. Corpus: 13,831 frames,
    5,157,609 opaque HUD pixels inside a bar, 2,702,931 of them changed by the bar (the rest the
    panels' own black).
  - **The fix: the HUD over the bars, as X-Men 2** (its widescreen.cpp bars_now / ws_mask_line:
    "the HUD's sprites stay on top of the bars"; R4 "as X-Men 2", and NOTES names its file the
    model). `ws_bars_paint` leaves every opaque pixel of a pinned piece (the panels, the boss meter,
    the GO! sign: hud_side 1, 2, 4, 5; not the x 0-15 cover), decoded from the frame's table and
    tiles after shift_vram (`hud_pixels`: size, flips, palette), kept per frame in `g_ws_bars`
    (`bars_keep`); high_fps's present path paints its interpolated frames from the same capture
    (present.cpp Cap.bars: the HUD is not interpolated). X-Men 2 keeps each piece's whole
    rectangle; here only its opaque pixels, so the bar stays black around the glyphs. Why not
    the other reading (pin each piece to the picture's inner edge where a bar is drawn): content_bars
    is a per-frame test (the street's left bar flickered in and out, ws-street), so a picture-edge
    pin would move the HUD 40 px whenever a bar comes or goes; over the bars the HUD keeps one place
    at the frame's edges on every screen, E3's "pinned to the screen edges" unchanged. The centre
    comparison is unaffected (a bar never reaches the 4:3 view's columns).
  - **The check**: on wide gameplay frames with a bar, every opaque pixel of a pinned piece inside a
    bar (walked again from VRAM at ws_frame_check) must be the frame's pixel after the bars as
    before them; a frame with one painted over counts as `hud_misplaced` (so E3 fails, no kit
    change), with `first_hud_misplaced` naming the pixel. ws JSON `hud_bars`: frames, pixels,
    hidden_frames, hidden_pixels, unmatched (the frame not showing the piece's own CRAM colour
    there before the bars: 1,428 of 5.16 M, a sprite in front or the line limit; a diagnostic) and
    `by` (screen, item, frames, pixels, hidden, px_into_bar). Control `BR_WS_PLANT_HUD_BARS=1`
    (the bars over everything again, the machine's frames and the present path's): the lift run
    hud_misplaced 3,158 = every frame with HUD pixels in a bar, scenario-level-skip 2,119, the
    corpus 13,831 in each mode; without it 0. high_fps: widescreen=16:9,high_fps=144 with
    HF_SELFTEST=1 on scenario-sec-2100-s1: 15,682 frames redrawn, 0 differ from R_k.
  - **E3 on 96f5280 + this** (1,317 functions, 172 runs, private dir, nothing cached): PASS (16:9
    218,983 gameplay frames, the gameplay mode 169,871, R21's pass 0 host misplaced, 0 HUD
    misplaced in all three); with `BR_WS_PLANT_HUD_BARS=1` (dkit.toml keyed_env: its own cache)
    FAIL "hud_misplaced 13,831" in both modes (R21's pass reports 13,831 too; it fails only on its
    host counters). sec-1301-crane adds no barred frames.
- **The checks (E3, `g_enh.ws`)**: terrain: the streamed plane's wide cells against the block map
  (every cell on lines whose H-scroll shows the layer at its place, plane x `layer_plane_x0` = col x 8
  + x - block_col x 16 at screen x 0; on lines scrolled apart, the flight's wobble and parallax bands,
  only the streamer's own cells, block columns -5..-1 and 22..26 of its rows: the rest of the plane
  is the game's, or no one's; coverage in the ws JSON: streamed_frames, streamed_lines,
  streamed_lines_apart, streamed_cells_left / _right), the planes drawn whole for a margin pixel showing nothing where the
  4:3 edge shows something (outside the bars: by construction 0, the bars come from the same test: a
  known limit); sprites_lost (a moved sprite the wide view shows outside the 9-bit x); hud_misplaced
  (a panel piece not at the game's x -/+ 40, or a pinned piece's pixel a bar painted over: above,
  "The pinned HUD over the bars"); bars_unneeded (a bar where the map spans >= 400 px).
  Controls: `BR_WS_CHECK_ALL=1` checks the 4:3 columns too (the model of the streamer: 0 errors on
  the corpus); `BR_WS_PLANT_COLUMN=n` writes a wrong cell into margin column n; `BR_WS_PLANT_VCOL=1`
  leaves column -3 at the clamp; `BR_WS_PLANT_HUD=n` pins player 1's panel n px short of its edge.
  Run through E3 by ws-fix (P6 run log). The street $1100's margin pixels against its facade
  model: `check_street` (ws-street, below), its errors terrain errors.
- **The terrain check was blind to the streamed margins until ws-ctl (2026-10-30)**: check_plane
  took a line as showing the layer where -H-scroll = col x 8 + (x & 15), but the game keeps a block
  of slack (Layer_DrawScreen draws block columns 1..22 of block_col, the right step queues +$15):
  block_col = (x >> 4) - 1 and the view's left is col x 8 + x - block_col x 16, 16..31 px into
  the block (measured: x 63 / 78 / 104 against -H-scroll 63 / 78 / 104, col x 8 32 / 48 / 80). No
  line ever matched: on scenario-level-skip 0 of 4,576 valid-layer frames saw a line, so every one
  of the 198,209,536 terrain checks in ws-fix's E3 was check_whole's, and BR_WS_PLANT_COLUMN=-1
  passed with counts equal to the clean run's (the plant was written, restream put it back before
  the check, nothing looked). Sighted (c826f26: layer_plane_x0, the coverage counters, lines apart
  checked in the streamer's own cells), the check found two things. (1) The streamer was one block
  short on the right where the view starts left of the map (the flight's entry $2400, layer x -340
  ..0): the render camera stops at +40, so x 399 is up to 79 px past block 21; 22,394 cells of
  block 26 on level-skip, $05FC where the map has $0360 (fix: STREAM_RIGHT 5, 7e4a8f6).
  (2) The flight $2300 wobbles every line a few px off the layer's x (no line at its place: 684
  frames unchecked) and starts on the cleared plane: its own columns hold $05FC where the map has
  $031A (both all-zero patterns, so it looks the same) until its scrolling restreams each column,
  ~340 frames; so on lines scrolled apart only the streamer's cells are checked. Left margin at the
  map's left end: the render camera's +40 puts the view's x 0 at the map's, x0 = 0, no left margin
  to check (by design). Coverage: only scenario-level-skip shows a streamed section in gameplay
  (the stepped movie stays in $1100, drawn whole): 4,576 streamed frames, 871,360 lines at the
  layer's place and 153,664 apart, 157,826 left and 775,482 right margin cells (16:9); E3 on
  9f7ab2e + these two commits: PASS (16:9 199,408,415 checks, gameplay mode 99,031,999, 0 errors);
  BR_WS_PLANT_COLUMN=-1 FAIL (terrain_errors 4,834 / 6,034), =22 FAIL (15,278 / 14,718),
  BR_WS_PLANT_VCOL=1 FAIL (edge_clamped_columns 293,664 / 293,664).
- **The centre (fb_centre)**: compared on gameplay rows with the 4:3 run. Listed exceptions, set
  alike in the 4:3 run from the game's state (`centre_exceptions`): x 0-15 when g_SceneFixedSpr
  (the cover moves away), and every HUD panel piece's rectangle both where the 4:3 view draws it and
  where the wide view pins it (enhance.cpp hashes `g_enh_centre_rects` as zero pixels). The HUD
  pinned to the edges cannot equal the 4:3 frame's centre (a contract conflict, run log).
- **What the view writes, put back (ws-fix, 2026-10-30)**: every VRAM word the view writes for a
  frame (the margin cells, the shifted H-scroll, the moved sprite x) is journaled with the game's
  value the first time (`pw16`) and restored byte by byte at ws_frame_check, except bytes the game
  itself wrote inside the window. machine.cpp's VRAM write paths (data port, DMA fill/copy) report
  them while `g_ws_window` is set (`ws_vram_touch`), and the game's VRAM reads and DMA copies in the
  window see its own bytes (`ws_vram_game8`). The game's VRAM after every drawn frame is the 4:3
  run's: scenario-level-skip 16:9 vs 4:3 is equal in every sync column (vram_crc32 included) on
  all 20,906 rows. Three leaks this closed. (1) The H-int writes the H-scroll table inside the
  frame: in the lift ($2100) the V-int writes $BC00 = $FFF8, the H-int $FEA8. The old restore
  copied the pre-frame table back over it, and `reshift_hscroll` now gives such words to the
  lines still to come through the render camera. (2) Line mode reads 224 entries (0x380 bytes),
  and the old shift touched 0x400: with H-scroll at $BC00, $BF80 is the clear tile $5FC's
  pattern, so while the camera shifted, every empty cell drew as stripes (the flight $2300 at
  the map's left end). Only the entries the VDP reads are shifted now. (3) The streamed cells
  stayed in VRAM, masked only while the registers made them a name table.
- **A line's 4:3 view keeps the game's cells (`centre_reclaim`)**: where a line's own scroll
  shows the streamed plane apart from the layer (the flight's per-line floor in $2300), its 4:3
  view can show the plane columns the streamer fills as margins. Before each line, every
  streamed cell that line's centre shows (its game H-scroll, live V-scroll) holds the game's word
  for that line, and the view's word again on the next line that doesn't show it (`restream`). At
  the frame's end the checks see the streamed margins. Counted as `margin_cells_given_back` in
  the ws JSON. For hfps-p6: VRAM now changes per line inside the window (reclaim, reshift), and
  the capture copies VRAM once at hf_display_begin.
- **The gameplay mode (R4)**: `WS_ACT()` = 40 with widescreen=16:9-gameplay on a wide screen:
  Trigger_RunX's window to camera + $140 + 40, the a4 kill windows (00555A Op4_KillOffScreen,
  006776 x2, 0370DC ToyFigure_FxSpinOrKill: g_OnScreenLeft - 40, width $170 + 80) and Op2_Native's
  kill boxes (x margin + 40). The render camera keeps the view on the map in both modes.
- **R11: the draw culls widened (ws-r11, 2026-10-30)**: `recomp/platform/native/ws_draw.{h,cpp}`
  and native branches in 004454.c, 007B72.c, 007D76.c (and the list's hooks 007A08 / 0079C0 /
  00ADE8). With `widescreen=16:9` (both modes) every draw decision that drops a sprite the 400-px
  view would show is widened, without the game knowing: the dropped piece is drawn by the host, in
  the margins only.
  - **The widened tests.** (1) `Op2_DrawInScreenBox` ($004454; 11 uses in the scripts: the pickups'
    common body $13358 and the drifting power-ups $13C14 x [-10, 330], the stage-1 warehouse and
    balloon scripts ($01F50C, $01F6C2, $01F7E8, $01F90E, $02042E, $020466) x [-32, 360], the flight's
    searchlights $02F756 / $02FCA2 / $02FE38 x [-20, 340]; y never widened): when the box culls on x
    and the box widened by 80 px a side (`ws_draw_reach`: 40 of margin + up to 40 of render camera)
    keeps it, the object's draw list is READ, not run (`ws_draw_list`: Op2_DrawSpr / Attr / Depth /
    AttrDepth / Off / AttrOff, the Front and Back forms, to Op2_DrawEnd: frame, position, attribute
    and depth per piece; the AtDrawn forms and anything else: not drawn, counted `unsupported`), and
    each piece becomes a culled node; Op2_Draw's blink is followed. (2) `Spr_BuildSAT`'s SAT_MAX_X
    ($160 = screen x 320; the unsigned compare also drops x < -32): the native second walk (sprites-p6's
    `walk_node`) sends every piece this clip drops and the wide view shows (screen x -112..399) to
    ws_draw, plus every piece of the culled nodes, merged into the walk where Spr_AddSorted /
    AddFront / AddBack would have put them (as spr_host's kept nodes). The original walk and its
    SAT_MAX_X are untouched, so the table, g_SprCount, g_SATPtr and the self-check (the walk's
    4:3 pieces against the table) are exactly the 4:3 run's. (3) `Spr_EmitClipped`'s
    g_SprDirectMaxX (the GO! sign sliding in, the debug digits): pieces past it go to ws_draw when
    the scene's window is the standard one ($160 / bias $60). Not draw culls, so not widened here:
    the a4 kill ops (Op4_KillOffScreen, Op4_DrawKill*: they delete objects, state: the gameplay
    mode's WS_ACT), Op2_BranchOffScrX and the "fires while on screen" tests (they change the
    script's path), activation and spawning.
  - **Budget of their own.** A margin draw calls no Spr_ChangeFrame and no Spr_Add*: no art cache
    entry, no VRAM block, no DMA, no display-list node, no table entry. A piece whose art is resident
    is drawn from the VRAM tiles the 4:3 view already holds (read only); a culled frame whose art is
    not resident is drawn from its art block (`SprFrame.art`: Spr_QueueArtDMA's source in ROM or RAM,
    chunk k at the queue's source step, the piece's tiles inside its chunk, else `unsupported`).
    The pieces go through the engine's host sprite layer (`gvdp_set_host_sprites`, called after every
    SAT and host-table sprite of the line): written only where no sprite drew (behind every table
    sprite, the x 0-15 cover included), only in the output columns outside the 4:3 view
    ([centre_x0, centre_x0 + 320) never), moved with the render camera, CRAM palettes, priority bit
    kept, shadow/highlight operator pixels skipped. Limits a frame: 128 culled nodes, 256 margin
    pieces (past: `nodes_lost`, `pieces_lost`), none of spr_host's 256 / 256.
  - **Timing**: the pieces of a walk are pending until Spr_QueueSATDMA queues that table (shown from
    then on, as the host table), the direct pieces of the frame before them; VDP_ClearSAT clears
    them; the state is in branch snapshots (machine.cpp, after spr_host's).
  - **The game's state: no mask needed.** Nothing writes work RAM, VRAM, CRAM, VSRAM or a register
    (the C only reads: the box words, the draw list, the frame tables, the art), so view_masks.h
    gets no R11 row: `--game-rows` compares the whole state and it is equal (below).
  - **Counters** (`[dkit] enhance` ws `margin_draws`): culled_nodes, nodes_lost, unsupported, pieces,
    pieces_lost, pixels (margin pixels drawn), frames. Controls: `BR_WS_DRAW_OFF=1` (no margin draws:
    the view before R11), `BR_WS_DRAW_PLANT=1` (margin pieces drawn over the 4:3 columns too: the
    centre check must fail). Measurement: `BR_WS_DRAW_LOG=DIR` (ws_draw.cpp: the kickoff's samples).
  - **Known limits**: a margin piece is behind every table sprite even where the game would have put
    it in front (only where a table sprite crosses into the margin); its pixels take no part in the
    per-line sprite limits; RAM-sourced art is read when the line is drawn, not at the V-int's DMA;
    Op2_Draw's shake jitter is not applied; still popping in the margins: objects created at ~x 333,
    deleted at the -24 line, or drawn first by a script that has not yet run its first draw.
  - **Measured, the named examples** (the kickoff's method on the native runner: `BR_WS_DRAW_LOG`, a
    wide corpus pass (this code less the Spr_EmitClipped pieces), every 4th gameplay frame, each a2
    object matched to the display nodes the walk drew, by frame in its slots and distance, or by
    owner for a culled draw;
    samples of one state replayed by several processes counted once: 21,356 samples). "In a
    margin" as the kickoff (extent wholly in x -40..0 or 320..360): 1,121 samples; with a display-list
    node 1,021 (the kickoff's "drawn"); shown by the wide view without R11 98 (table pieces in x
    -32..0 only: SAT_MAX_X drops everything right of 320 and left of -32); with R11 1,010. Wholly
    outside the 4:3 view and inside the render camera's reach (x -80..400): 3,880 samples, 533 shown
    before, 3,557 after (70 by culled draws). By object (reach samples, shown before / after):
    stage-1 clowns (Enemy_Clown_Spawn) 1,450: 250 / 1,415; trash cans (Stage1_SpawnTrashCan, made at
    ~x 333) 1,325: 75 / 1,325; brutes 393: 129 / 393; drop-in gunmen 188: 65 / 162; drifting
    power-ups (Item_SpawnPowerUpDrifting) 50: 0 / 50; the flight's searchlights
    (Flight_SpawnSearchlight, Op2_DrawInScreenBox x [-20, 340]) 285: 0 / 64 (39 by culled draws;
    they flicker: the kickoff's on-screen control is 69% undrawn); the $1301 fly-ins (Scr2_01F898,
    box x [-32, 360]) 49: 2 / 48 (27 culled), Scr2_01F64A 5: 1 / 5 (4 culled). **Not in the native
    corpus**: Harley's jet ($1301) and the stage-2 lane jets / biplanes / pipe cannons ($2300,
    $2400): the native movie stays in stage 1 and scenario-level-skip spends ~560 idle frames per
    section, before they spawn (0 instances); a run to them needs a reference run and its D1 trace,
    which a private artifacts dir refuses to write (shared traces). Their scripts have no box: the
    jet's loop (Scr2_01EE64) runs Op2_Draw at $01EF16 every frame and the lane jets' body ($0285DE)
    at $028628, so when they are in the display list R11 draws their margin pieces (the clip's case,
    from the VRAM tiles they hold). Why the kickoff found them without a list node in the margins
    (Harley 151 of 154 samples, the lane jets 45 of 45) is not known: not a draw test of the kinds
    above (a script path, a blink, or the kickoff's matching); not verified natively.
  - **E3 with R11** (`tools/phase.py 6 --draft --only E3`, shared artifacts, e5d73e2 before its rebase onto f91e5e4, kit 0.11.2;
    1,694 s: the 4:3 pass 514 s, presentation 483 s, gameplay mode 552 s): fails only as before R11
    (ws-fix's): the presentation mode's game_crc32 differs in scenario-level-skip from sync 3352 and
    its fb_centre on 192 gameplay rows there; the gameplay mode stops in scenario-level-skip at sync
    8391 ($0391AA uncompiled). Every other run: game_crc32 equal at every sync point and fb_centre
    equal on every gameplay row with the margin draws on (153 of 154 runs, the two passes' rows
    compared by hand), 0 terrain errors, sprites lost, HUD misplaced, edge-clamped columns. Controls:
    `BR_WS_DRAW_PLANT=1` (margin pieces moved 48 px into the view) on the movie's first 20,000 frames:
    game_crc32 equal, fb_centre differs on 3,359 gameplay rows (the centre check sees a margin draw
    that reaches the 4:3 view); `BR_WS_DRAW_OFF=1`: the margin pixels go (a wide frame at f2609:
    15 left-margin pixels differ, 0 centre).
    **On main 1a28113** (ws-fix's f91e5e4 and the 1,154-function tree; private corpus passes, 4:3
    and widescreen=16:9, `--game-rows --hash-centre`, compared by hand): game_crc32 equal at every
    sync point and fb_centre equal on every gameplay row in all 154 runs; the margin counters over
    the corpus: 332 culled nodes, 0 unsupported, 0 nodes or pieces lost, 353,562 margin pieces,
    21,579,858 margin pixels on 63,561 frames. (The kit's E3 rerun was stopped: with the movie run
    cached its pass plays every run alone, 141 processes and 9.9 M frames; a kit point.) Gate
    `phase.py 5 --only P1 P7` on that tree: PASS.
- **The width sites, classified (ws-tail, 2026-10-30)**: the kickoff's 34 files (its width scan,
  `p6-measure-ws/width_scan.log`: the camera-relative constants and every width-like one) and a
  second pass over every `g_CameraX` / `g_ViewX` / `g_OnScreenLeft` use in the hand C. Draw =
  presentation (the view changes it, the game never sees it); state = what the game decides
  (kept in the presentation mode, R4; widened in the gameplay mode only through `WS_ACT()`, 0 in
  the PC modes and with the option off).
  - **State, widened in the gameplay mode** (ws-p6): Trigger_RunX 012E02 (activation), Op4_KillOffScreen
    00555A, Op4_DrawKillSidesBelow / OffScreen 006776, ToyFigure_FxSpinOrKill 0370DC (a4 kill
    windows), Op2_Native's kill_unless_inside 0007FC. **Now too** (ws-tail): `Op4_ShotStepAt` 006952
    (a shot is freed off camera: x - camera >= $140, now -40..360), `ToyChain_SteerAtPlayer` 037A6A
    (a chain whose both ends are past screen x -100 / $1A4 is removed: -140 / $1CC), and
    `Flight_AirshipRunTriggers` 02F58C (the flight's trigger window [camera + map_x, +$17C], the
    airship's Trigger_RunX: +40 as Trigger_RunX).
  - **Draw, done**: Spr_BuildSAT's SAT_MAX_X 007B72, g_SprDirectMaxX ($160, Scene_InitCommon 009AAC) /
    Spr_EmitClipped 007D76, Op2_DrawInScreenBox 004454 (R11, ws-r11); the GO! sign's start x $168
    (HUD_StartGoSign 0119EA, HUD_UpdateGoSign 011A1C) and the boss meter's centre offset $A2
    (BossMeter_Init 011C5E): pinned above; the Cat head's face / pivot x $A0 (03B91E, 03BB88: the
    raster's centre, R12's columns continue it); the road's H-scroll centre $A0 (Stage3_RoadCameraFollow
    04062A: the render camera shifts it); BatmobileIntro_Animate's BUMP_X -$140 (04A1C8: plane A 320
    px over for the bump, on the opening, a 4:3 screen with bars); the stage card subtitle entering
    at x 320 (04A2A8, a 4:3 screen).
  - **State, kept in both modes** (not windows that create or remove objects): the camera locks
    (Camera_LockHere / Camera_LockRightEdge 0093A0, Op5_LockCameraHere 006E74: R4 "the camera keeps
    its limits", R16 shows the level past them); Camera_TrackPlayersX's centre $A0 (00943E); the road
    camera's aim (players - 160) and limits $48-$110 (04062A); the players' movement area (0093DC
    g_MoveAreaLeft = camera + g_MoveAreaOfsX) and EnemyAtk_ApplyKnockSpeed's push inward within $30 /
    past $110 of the screen (0105AE: it keeps the players inside that area); screen-placed wave
    entries (VM5_SpawnEntry 006FEA: y in the $E000 range = camera-relative; their courses fly fixed
    distances from there); the script-placed screen positions and screen-x branches whose bounds are
    script operands (Op2_SeekScreenX / Range / OrBranch 00194E, Mot_SeekScreenX 001922, Op2_PlaceOnScreen
    00156A, Op2_BranchOffScrX / Box 0007FC: per-script meanings, a turn, a stop, a shot); the police
    car starting 80 px left of the screen (01E58C: already outside the wide view). The two gameplay-
    mode ones a player would notice (screen-placed spawns popping in at x 320-360, the players kept
    in the 4:3 columns) are queued in QUESTIONS.md (default: kept). The user chose to widen both
    (R20): the gameplay mode now does, below ("R20", ws-play).
  - **Not a width**: the screen centre ($A0, 160 px: the wide view's centre is the 4:3 one's):
    Spawn_AtScreenCentre 008CCA, Op2_FxPosScreenCentre 0051F2, Op5_OriginAtScreenCentre 006E18, the fx
    at 0091F2, Op2_TargetRandomScreen 001736 (centre +-75 / +-55), Boss_Harley_PickDir8 01A04C (left or
    right of the centre), the weapon drops' x + $A0 (013F5E), the rubble rain (020A48, centre +-150),
    the bombing's fire window x $A0 (020D76, a facade window's place), the debug cursor (0156FC,
    015730); world-space periods: the floor segment $168 (Coll_SetFlatFloor 010E0A), the facade
    segment $168 (015D90, 020866); lines and y: SPLIT_SCREEN_LINES $A0 (01C3F2), the hall floor's
    lines 160-223 (047674), GATE_DRAW_Y $140 (04857E), REVEAL_SCREEN_BOTTOM $170 sprite y (03CEF8),
    plane-B line offsets (04788C); other quantities: the glyph mask offset $140 (011C5E), the panel
    text's -$160 byte stride (011710), LADDER_PULL $180 (0268F2), FLIGHT_SCROLL_STEP $180 (033274),
    CHAIN_SWAY_DIV $A0 (03730E), UPPER_FACTOR $180 (0477D4), STEP_DIVISOR $160 (047842); the biplanes'
    "160 px in" courses (02D886, 02DA76, 02DA92, 02DD14: comments).
- **The host table under the render camera (ws-host, 2026-10-30)**: with `unlimited_sprites=1` the
  sprites past the 80-entry table are drawn by the engine from `gvdp_set_host_sat` (spr_host.cpp),
  which shift_vram never saw: every VDP table sprite moved -shift for the frame, the host ones stayed
  at the game's x, so wherever the render camera shifts (a streamed map's ends, R16) they sat up to
  40 px off the rest. Now every send to the engine goes through a view (`spr_host_set_view`):
  shift_vram installs `host_view` for the frame and unshift_vram removes it, so the engine draws a
  moved copy (`spr_host_drawn`) while the game's table (`spr_host_shown`) is never touched (no game
  state, as the VRAM moves). One move rule for both tables (`sprite_move`: the HUD pinned by
  `hud_move`, the cover to the wide edge, a VDP-x-0 sprite kept, the rest -shift); a host sprite's
  HUD item is by attribute only (`hud_side_attr`: no table entry for a writer's tag). The panels,
  the boss meter and the GO! sign are written straight into the SAT buffer, never display-list
  nodes, so they never reach the host table: nothing pinned is moved twice. R11's margin pieces
  (ws_draw) were already drawn at x - shift (draw_line), as the table's.
  - **The check (`check_host`, every frame drawn with widescreen on)**: each host sprite as drawn
    against the game's moved by `sprite_move`, and the engine's count against the one sent:
    `host_sprites_checked`, `host_sprites_misplaced`, `host_shift_frames` (frames with a host table
    and a nonzero shift) in `g_enh.ws` (the enhance JSON; `first_host_misplaced` beside the other
    first errors); on gameplay frames the 9-bit test adds to `sprites_lost` as the table's.
    **No criterion runs the two options together** (E3 runs widescreen alone, E5 unlimited_sprites
    alone; changing their passes is the user's), so the counter is reported, not yet checked.
  - **Where the gap shows: nowhere in the native corpus.** A combined pass (`--options
    widescreen=16:9,unlimited_sprites=1 --render`, 154 runs, 374 s): 6,661 host sprites in 6 runs,
    all on frames with shift 0: the game-over snow (tas, fuzz-10800-s2, fuzz-91800-s1/-s2,
    fuzz-90000-s1: the skyline is not a streamed map) and section $1100's fights (fuzz-90000-s1,
    scenario-two-player: drawn whole); `host_shift_frames` 0, `host_sprites_misplaced` 0. The busy
    fights that overfill the list at a map end (BizHawk's cov13-hat2p*, the stage-3 run) are not in
    the native corpus (its movie desyncs in stage 1: "Phase 6: unlimited sprites").
  - **So the shifted case is made**: `BR_WS_HOST_MIRROR=1` (a diagnostic, with unlimited_sprites)
    draws every VDP table sprite that is not the HUD or x 0 again from the host table, behind itself;
    check_host compares each copy with its table twin as drawn (VRAM after shift_vram).
    scenario-level-skip (every stage, 20,906 rows, `--fb`): 159,598 host sprites checked, 3,305
    frames with a shift, 0 misplaced, every frame hash equal to the run without the mirror (the
    copies cover their twins exactly). With `BR_WS_PLANT_HOST=1` (the view skipped, the gap as it
    was): 26,194 misplaced (first: section $1300 frame 1301, a copy at x 128, its twin at x 88, shift
    40) and the picture differs on 3,055 rows; the other sync columns equal on all three.
  - **high_fps** (present.cpp): the capture takes the drawn copy; the host table's tags compare
    without x under widescreen (as the table's); an interpolated frame draws the captured host
    table (moved toward R_(k-1) or not) and puts the machine's back (`spr_host_redraw`): outside the
    display the machine's is the game's, unmoved. At 144 Hz with both options and `HF_SELFTEST=1`:
    scenario-level-skip with the mirror 50,237 frames redrawn, 0 differ from R_k, 0 line-by-line
    differences; tas' first 15,000 frames (the game-over snow) 36,044 / 0, host displays 497 / 497
    tagged and 681 frames in between moving host sprites (hfps-2's 4:3 numbers, now in 16:9).
  - **Checks on 4509d23** (its tree; tested before the rebase as 0d161ff; private artifacts, nothing cached): `phase.py 5 --only P1 P2 P7` 3/3
    (P7 306 s, P1 747 s: 154/154 runs, 10,175,742 native sync points; P2 160 s); `phase.py 6 --draft
    --only E3 E5` 2/2 (E5 1,213 s; E3 1,026 s: 198,209,536 / 97,856,416 terrain checks, 0 errors);
    `--only E4` PASS 1,732 s (passes 419 / 423 s, movie 395 s, latency 340 s, render p99 1,869 us).
    E3 and E5 each run their option alone: the combined pass is queued for the user (QUESTIONS.md).
- **The street's margins (section $1100, ws-street, 2026-10-30)**: the user's first look (16:9
  presentation, high_fps=144, unlimited_sprites=1): "messed up visuals on the sides of the screen
  in the widescreen periodically as the stage scrolled". **What it was.** The street has no
  streamed layer and does not scroll a plane through a map: Stage1_RunStreet draws the facade once
  into plane B (128 x 32 at $E000) from the map buffer g_BufPtrs[1] (rows of $D8 bytes): picture
  columns 0..83 (672 px), then the sign panels' source cells (columns 84..107: every map offset in
  Tbl_015CF4 lands at column 84 or 96), then the clear tile (108..127). The facade repeats every
  $168 px of facade x (g_ViewX x 35/32); Stage1_UpdateStreetScroll keeps the offset in the segment
  g_Stage1SegOfs in (-$B4, +$B4] px and Stage1_ScrollFacadeLines turns it into plane B's line
  scroll in five bands of shear (L = 175.5 - offset x b_line), so each line repeats with its own
  period P = 360 x b: 360 at the top, 324 at band 1's end, 288 at band 4's bottom (the sign row:
  slots 0 and 2, 1 and 3 are 38 cells = 304 px apart). At a segment change the offset jumps by $168
  and each line by its P; the picture goes on. Over the offset's range a line's 4:3 view stays in
  [L(+$B4), L(-$B4) + 320) = about [-4..32, 640..676) of plane x, so near each segment's end the
  40-px margins reach past the facade: on the right into columns 84..89 (the blasted-sign source
  art: black spiky shapes), on the left into columns 122..127 (the clear tile: the backdrop on
  every line, which content_bars barred black, a bar flickering in and out). Plane A (the sky's
  three 341-px cloud copies, the fire) wraps cleanly. Measured on the movie (BR_WS_SHOT_VRAM, every
  30th street frame of its first 6,400): a margin past the facade on 11 of 199 frames (7 left, 4
  right); e.g. frame 489 right margin on all 224 lines (the sign art beside the 4:3 doors), frames
  609 / 2739 / 2941-2950 a black bar on the left, 2932-2938 the art on the right. E3 passed: check_whole
  asks only whether something is drawn in each margin column (the art is; the bars are outside it).
  **The fix (R4: the view only).** A margin pixel whose plane x leaves its line's 4:3 range
  (clamped to the facade's [0, 672)) is drawn from the game's own line scroll one segment on (right:
  offset + $168) or back (left: offset - $168): the same facade point as the game draws it after or
  before the segment change. widescreen.cpp has a replica of Stage1_ScrollFacadeLines (divs.w, the
  16.16 ramps, the muls.w on band 1's step), which must equal the game's table on every checked
  frame (it did on every frame measured). A sign panel a folded pixel lands on is drawn from its
  source cells (as Stage1_DrawSignPanel, + $23122312) in the state of the sign it stands for: one
  segment on, slots 0-1 stand for 2-3 (drawn states) and 2-3 for the segment after next (intact: the
  game clears 2-3 when the segment grows); one segment back, 2-3 stand for 0-1 and 0-1 for the
  segment before, whose drawn states the view keeps from the frame before the change
  (`WsSaved.street`, in branch snapshots). The pixel is composited with plane A and the backdrop as
  the VDP would (priority; shadow/highlight is on in the street, reg 12 $89: shadowed where no
  plane pixel is high) and drawn through the host sprite layer (`ws_street_line`, called by
  ws_draw's draw_line after R11's margin pieces): only where no sprite drew, as a high sprite
  pixel where a plane pixel is high, else a low one (the VDP shadows it and applies operators as
  for the plane). The game's VRAM, RAM and timing are untouched (no journal entry); content_bars and
  check_whole read plane B through the fold (`pixel_drawn`). high_fps: the interpolated redraws
  call the same layer, so the fold follows the interpolated line scroll (HF_SELFTEST below).
  **The check (`check_street`, every wide gameplay frame of $1100).** Every margin pixel of the
  drawn frame against the model (plane A as drawn, plane B at the line's plane x inside its 4:3
  range, else one segment on or back with the signs above, composited, the palette's shadow
  variant where no plane pixel is high), compared as ARGB with the machine's palette. Left out and
  counted: pixels a sprite drew (the layer's opaque columns before the fold, recorded on the
  machine's own frame) and, with shadow/highlight, every pixel a palette-3 sprite's box covers
  (operators shade what is under them). A line whose plane B scroll word is not the replica's is an
  error too. Coverage in the ws JSON `street`: frames, pixels, folded, sprite_pixels, errors
  (errors also count as terrain errors; `first_error`). Controls: `BR_WS_PLANT_STREET=1` (no fold:
  what the user saw) and `=n` (the fold n px off its period).
  **Measured.** The movie's first 6,400 frames (16:9): 5,981 street frames, 100,616,798 pixels
  compared, 1,681,055 folded, 6,562,722 under sprites, 0 errors; PLANT_STREET=1: 1,216,550 errors
  (first: frame 469, x 359 line 141, CRAM 30 wanted, folded right); scenario-two-player: 1,639
  frames, 27,148,317 pixels, 729,513 folded, 0 errors; PLANT_STREET=8: 189,395 errors; the gameplay
  mode 0 errors (555,520 folded); widescreen=16:9 + high_fps=144 + unlimited_sprites=1 with
  HF_SELFTEST=1: 0 errors, "6012 frames redrawn, 0 differ from R_k" and 0 line-by-line differences
  (the fold in every interpolated redraw). E3 on 44bd00c + this (private dir, --fresh, 171 runs):
  16:9 terrain errors 0 (the movie alone: 15,065 street frames, 257,524,171 pixels, 3,723,329
  folded, 0 street errors), game_crc32 equal; the gameplay mode 1,060,172,777 terrain checks, 0
  errors; with `BR_WS_PLANT_STREET=1` E3 FAILS on terrain_errors 16,361,728 / 2,907,363. E3 itself
  still fails on fb_centre in scenario-sec-2100-s1/-s2 (2,792 gameplay rows each, the lift $2100
  from sync 3736: the same rows with origin/main's widescreen code, so not the street's; new runs
  of corpus-p6's). Frame by frame across a segment change during the bank
  bombing (movie 2715-2758, blasted signs on screen): the margins continuous. Not changed: the fire
  (plane A) leaves through the left margin and is gone once its anchor is $A0 left of the facade x
  (Stage1_SwayFireLines stops writing its lines), past the wide edge by then. (Wrong: its right end
  and the sway's lean stay inside the margin, frozen there: ws-balloon, the next item.)
- **The street's fire, frozen in the left margin (ws-balloon, 2026-10-31)**. The user's report
  (the package of 372ef7b, widescreen=16:9 presentation, high_fps=144, unlimited_sprites=1): "visual
  issue on the top left", a screenshot at "BOSS TIME" (score 23020, timer 85) with garbled dark
  grey/black pieces with orange edges at frame x 0-30, lines 0-80, in and under player 1's panel.
  The scene is **the Harley mid-boss** at the street's end (the balloon-topped cart,
  Boss_Harley_Spawn, camera locked at 4935), not the Joker balloon (the truck's end; its balloon is
  green/red/orange): the brief named the wrong boss.
  - **Reproduced**: the native movie reaches it (it desyncs in the street and fights Harley from
    runner frame ~4,500 to its game over: tas is a corpus run). Rendered with the user's options and
    BR_WS_SHOT_VRAM every 20th frame: a dark strip at frame x 0-7, lines 0-75, on every Harley
    frame; at movie frames 3,203-3,212 a 30-px-wide black smoke shape there (the plant, below). The user's
    run froze it further right (x 0-30, inferred from the screenshot). A closed-loop drive through the fight (from the
    movie's state, a local RAM poke for the player's health) and the debug pause menu's free
    camera (g_DebugPause poked: no writer in the game) to reach $1100's and $1400's ends were used
    to look around; neither is committed.
  - **What draws it: plane A, the building fire** of the bank bombing. Not a sprite (the frame's
    SAT has nothing at frame x 0-7; the same pixels with BR_WS_DRAW_OFF=1, with the street's fold
    off, with widescreen alone: high_fps and unlimited_sprites play no part). Stage1_VIntFireStep
    animates the fire in plane A's rows 0..9 in two copies, columns 0..13 and 64..77 (112 px, 512 px
    apart; Tbl_015A7E's 24 entries write one copy in four parts while the other is shown, the
    entry's x offset g_SceneVars+4 = 0 / 512 picks it). Stage1_SwayFireLines writes plane A's lines
    0..$4F - g_ViewY: the fire's place (fire x - facade x, x 32/35 by divs/muls, rounded, + the
    offset) with a sine lean per line, and **stops once fire x - facade x <= -$A0** (~146 px left of
    the screen): the 4:3 view no longer shows it. The lines keep their last words for the rest of the
    street (the Harley fight then sets g_Stage1FireState $FF, which stops the line effects), while
    the animation goes on alternating the copies under them. On the movie's Harley frames the words
    are 368-372 (the lean: 4 px over 80 lines): copy B at x -140..-28 of the 4:3 view, so frame x
    0..12 of the wide view show its right end, a different copy's tiles every few frames. The lean
    can reach ~30 px over the band (the user's: 30 px at the top, less below: the diagonal).
  - **The fix (widescreen.cpp fire_now, R4: the view only)**: past the cut, the fire's lines take
    the words SwayFireLines would write (its arithmetic: divs.w with the overflow rule, the 16.16
    lean from Tbl_059B22 at the phase as it stands; the drawn lines' offset), so the fire keeps
    leaving through the margin with the facade; per line, once the shown copy's x is past
    -m - 112 it is held there: the copy's right end at the wide edge and the other copy (512 px on)
    at x 360, just past the right edge (the copies' gap, 400 px, is the wide view's width). Written
    as journaled VRAM words while the frame is drawn (as the render camera's), so RAM, VRAM after
    the frame and timing are the 4:3 run's; the 4:3 columns show plane A's clear tile at either
    word (rows 0..9 hold only the copies). **The offset's timing**: the V-int DMAs the lines the
    last logic frame wrote, then Stage1_VIntFacade steps the fire, so at the frame's setup RAM holds
    the next offset; the drawn lines took the one RAM held at the previous drawn frame's setup (kept
    in the branch snapshot, StreetSaved.fire_xoff). Measured on the movie's street before the cut:
    2,078 frames, every one matched by the previous setup's offset, 519 not by RAM's.
  - **The check (check_fire, E3)**: every wide gameplay frame of $1100 with the fire (state 1 or
    $FF): each fire line's plane A word as drawn against the model (before the cut the game's own
    words, so the replica is held to them; past it the view's): fire_errors; and check_street's pixel
    model takes plane A at the model's words on the fire's lines, so a fire left frozen is a pixel
    error wherever it shows. Both are terrain errors (E3 fails, no kit change). ws JSON
    `street.fire`: frames, continued (past the cut), lines, errors, first_error. Control
    `BR_WS_PLANT_FIRE=1` (the frozen words, what the user saw).
  - **Measured**, the movie's first 9,000 frames: 7,021 fire frames, 4,942 past the cut, 560,861
    lines, 0 errors, street errors 0 (16:9 alone; and with high_fps=144 and unlimited_sprites=1,
    HF_SELFTEST=1: 21,625 frames redrawn, 0 differ from R_k, 0 line by line). With
    BR_WS_PLANT_FIRE=1: 394,627 fire line errors and 770,745 street pixel errors (first: frame 3,201,
    x -40 line 0). Frame by frame at the cut (movie 3,197-3,212): the smoke leaves x 0-30 in ~9
    frames with the view's words; frozen without them. **E3** on 9d28708 + the fix (private dir,
    nothing cached, 172 runs, 1,953 s): PASS (16:9 1,977,902,217 terrain checks, the gameplay mode
    1,093,043,509, R21's pass, 0 errors in all three); the fire on the corpus is the movie's (tas and
    its six branch prefixes; per process 8,243 fire frames, 6,164 past the cut, 658,583 lines, 0
    errors; the gameplay mode 14,245 fire frames, none past the cut: its fight parts from the
    movie's). With `BR_WS_PLANT_FIRE=1` (keyed env, its own cache): FAIL "terrain_errors 7,183,036"
    in the presentation pass and R21's pass. No new corpus run was needed: the native movie
    already fights Harley with the fire frozen.
- **R20: the gameplay mode widens the spawns and the players' area (ws-play, 2026-10-30)**. With
  widescreen=16:9 and widescreen=16:9-gameplay only: every hook is 0 in the PC modes, with widescreen
  off and in the presentation mode (E3's game-state equality and phase 5 see the original;
  scenario-sec-2100-s1 16:9 and 4:3 rows byte-identical to main's binary).
  - **The wide view, for the game (widescreen.cpp `view_extent`)**: [left, right) in the 4:3 view's
    screen x = the render camera's window (`shift_for`) less the bars (`content_bars`): -40 / 360
    mid-level, 0 / 400 at a streamed map's left end, -80 / 320 at its right end, 0 / 320 where a
    picture drawn whole ends at the 4:3 edge (3101's arena). From the machine's state (registers,
    VRAM, RAM; the bars' V-scroll margin columns at the clamp, `s_vs_logic`, not the last drawn
    line's fill), once per sync interval (the game's first call fills it, a drawn frame never
    does), recomputed after a snapshot load: the game decides the same drawn or not, and a restored
    branch decides as the run it came from. Written nowhere.
  - **Screen-placed spawns, for the wide view's edge** (dk_ws.h `ws_place_x`: x >= 320 moved out by
    what the view shows right of 4:3, x < 0 by what it shows left; inside unchanged), so an object
    starts as far past the wide edge as the original's start was past 4:3's. Static scan of every
    `x, y, action<<24 | spawner` entry naming a spawner: 1,003 screen-relative entries, 414 right
    (x 320-416, most 336-352), 214 left (-112..-16, most -48..-32), 375 inside (drop-ins at x 160
    and the like). The sites: VM5_SpawnEntry 006FEA (the screen-relative entries, y in the $E000
    range, by the entry's x plus the wave origin; and a world entry while the camera is locked at
    x 0, `fixed_screen`: the flights and the fixed arenas, where world x is screen x; the flights'
    waves set their origin to x $154 = 340, Op5_SetOrigin / SetOriginX $22108 / $23CFC..., and 33
    biplanes and drones of the private pass were made there: Enemy_Biplane_SpawnZigzagDown /
    ZigzagUp / WideDown, Enemy_RedDrone_SpawnLoops, $2300); Spawn_AtCameraOfs 008CCA (3 uses:
    Boss_HarleyJet_SpawnFinal at x -30 and Boss_HarleyJet_SpawnSecond at x 350, $1301, both
    margin pop-ins before; the forest robot at x 0, kept); Op2_PlaceOnScreen / KeepVel /
    PlaceXOnScreen 00156A (48 + 10 + 7 operand sites; in a margin: x 350 at $036652, $03695E,
    $036A22, $0390F4 (stage 3), 360 at $02EA40, 340 at $01E97E; past it 400 at $018084, -88 at
    $025C60); Stage1_SpawnPoliceCar 01E58C (-80). Not screen-placed: world entries elsewhere
    (their waves start from Trigger_RunX, already widened), Player_JoinOrSpawn's drop-in and
    respawn (on screen), script screen targets (Op2_SeekScreenX..., kept by ws-tail), the stage-1
    section exit Stage1_SpawnSectionExit (x 305, box +-15: inside 4:3, still reached).
  - **The windows follow the view per side** (`ws_win_left/right`). WS_ACT's 40 a side put a spawn
    moved 80 at a streamed map's left end (330 -> 410) on Op2_KillOffscreen50's edge ([-90, 410)),
    and removed objects visibly inside the 80-px left margin at a map's right end. Every R4 site
    widens by the view's own margins now (40 / 40 mid-level, as before): Trigger_RunX 012E02, the
    flight's 02F58C, 00555A, 006776 x2, 006952, 0370DC, 037A6A, 0007FC's kill boxes.
  - **The players' movement area = the wide view** (Op2_ClampMoveToAreaX 004B74: g_MoveAreaLeft
    out by the view's left margin, the width by both; the RAM variable stays the game's, nothing
    else reads it). The player scripts run it every frame (Batman $04AD26, Robin $04D862; the whole
    clamp at $04CF84, $04D220, $04FAC0, $04FD5C), as do two enemies ($02582E stage 2, $03C734 the
    forest robot), which now reach the margins too. Kept with it: the knockback's inward push near
    the edges (EnemyAtk_ApplyKnockSpeed / HitKnock 0105AE: within $30 of the wide left edge, from
    $110 + the right margin) and Op2_HoldRangeOfPlayerX's window clamps (002FF6: 10 / 20 and 300 /
    310 widened likewise, so an enemy holds its distance from a player in a margin as from one at
    the 4:3 edge, and walks round to the player's other side the same way).
  - **The arena locks** (Camera_LockHere / Camera_LockRightEdge, R16's render camera). At a lock
    the camera stops and the view shows the map past the lock's 4:3 edges; the players now walk up
    to 8 px from the wide edges, past the lock into the level R16 shows (the terrain there is the
    map's; past a map's own end the render camera's clamp and the bars bound the view, so the area
    never reaches beyond the map's ends on a streamed map; on a picture drawn whole the block map
    clamps its index at 0). Kept in view by construction (the area is the view): on the corpus
    39,137 player-frames in a margin, 38,743 of them with the camera locked (the arenas: the
    players stand past the lock's 4:3 edge), against 69 / 69 before R20; 80 player-frames outside
    the wide view against 82 before (scripted moves, the same 2 runs), so none comes from the area. Nothing that decides the fight reads
    the player's x against the 4:3 edge: every trigger table is keyed on the camera (Trigger_RunX,
    the flight's window), the camera only follows the players' mean once the lock clears (GO!: the
    camera passing g_GoSignCameraX), the section exit is a touch box at x 305, the exit walk
    (Player_SetExitFlag, var1 $800) runs on timers (Op2_SetTimer3 $140) and screen-x switches
    toward x 80-160 (Op2_SwitchScrX), and the player script's off-screen test (Op2_BranchOffScrX
    at $04CFCC / $04FB08) is [-50, 400], outside the wide area.
  - **The measurement** (ws JSON `play`; BR_WS_PLAY_LOG=path logs each placed object and its fate
    per process; BR_WS_PLAY_TRACE=spawner one spawner's objects frame by frame; BR_WS_PLAY_OFF=1 /
    2 / 4 the spawns / area / windows as before R20, 7 all): placed_moved / placed_kept /
    placed_unmoved; player_margin_frames, player_margin_locked, player_offview_frames;
    followed_ended / followed_never_in / followed_offview_long (alive 600 frames outside the view);
    locked_frames, lock_long (a lock held a minute). Each object placed outside 4:3 is followed
    until its block's serial or handler changes (a handler change also ends a follow: "gone" is
    not always a death, so objects followed fewer than 40 frames are left out below).
  - **Measured** (the 171-run corpus of 44bd00c, private dir; the gameplay pass of E3 on b784958 (d6d8c39 after the rebase) against the
    before-R20 control `BR_WS_PLAY_OFF=7`, same options and flags). An object "reached the view"
    when its x range met the view the player had: the wide view widened, the 4:3 columns in the
    control (what the original showed). Placed, followed >= 40 frames, reached, the median life
    (widened | control): stage-1 clowns dropping in (Enemy_Clown_SpawnWeak, $1100) 65, 100%, 551 |
    25, 84%, 181; running clowns (Enemy_Clown_SpawnRunIn, $1100) 38, 97%, 258 | 27, 96%, 291;
    walk-in gunmen ($1100) 12, 100% | 8, 100%; drop-in gunmen (Enemy_Gunman_SpawnDropIn, $1400)
    17, 94% | 11, 91%; the $1301 propeller drones (Enemy_Copter_SpawnRedFast / GreenFast) 66, 100% |
    58, 100%; toy figures (ToyFigure_SpawnStopHop, $3100 / $3200) 77, 96% and 32, 100% | 71, 93%
    and 57, 98%; the police car, the stage-2 hover drones ($2200), the lab techs ($4300), the jet
    troopers ($4400) and the rest 100% in both. No spawner reaches the view less often widened than
    the original reached 4:3: an enemy whose course flies a fixed distance from its start (the
    drop-in clowns, the drones' 100-px drops, the fly-by toys' 300-px pass) ends 40 px further out
    and still inside the wide view. One object of 400+ widened stayed 600 frames outside the view
    (0 in the control): a running clown ($1100, camera unlocked) that waited 1,791 frames at x 400,
    just past the wide edge, then ran in (in view 287 frames, then gone); its course waits
    off-screen in the original too (placed at x 360, past 4:3's edge). The flights' screen-placed world entries ($2300, moved 12-40 px: the render
    camera shifts there): 26 biplanes and drones followed (Enemy_Biplane_SpawnZigzagDown / ZigzagUp
    / WideUp, Enemy_RedDrone_SpawnLoops), 100% reached the view. The cost is the arrival: the
    first sight comes 8-80 frames after creation (the drones 8-15, walkers ~20-30, drop-ins ~75,
    toy figures 52-68) where the control showed the object at once inside the margin: the pop-in
    R20 removes. Totals (ws JSON `play`, widened | control): 610 placed and moved | 0 (633
    outside 4:3 unmoved), 133 | 170 inside 4:3 kept; locked frames 101,334 of 183,017 gameplay
    frames | 81,985 of 161,506 (the gameplay mode diverges from the movie, so the fuzz runs start
    from different states: per run not comparable). Runs with a lock held a minute: 14 widened, 7
    in the control: the flights ($2300 / $2400, 4 runs, both: the flight's camera is locked by
    design) and $4401's Freeze arena; scenario-two-player-exit, traced (BR_WS_DUMP): widened, the
    first arena clears at frame 2392 against 4192 in the control, then the scenario's fixed inputs
    lose the second arena and the game is over at 8736 (the control's are lost after the first, at
    9272), a lost fight, not a stall; 8 fuzz runs (random inputs from the movie's states), not
    traced one by one. No placed object of either pass stayed 600 frames outside the view under a
    lock.
  - **Not reached by the native corpus**: Harley's jet ($1301 boss: Spawn_AtCameraOfs placed 0
    times), the stage-3 PlaceOnScreen sites at x 350, the flights' lane jets
    (Enemy_Jet_SpawnMissileLauncher: 2 objects in the control, 0 widened).

## Phase 6: high fps (R3, E4, hfps-p6, 2026-10-30)

`high_fps=H` (120, 144, ...; F9 in played runs) shows the frames of an H Hz clock between two logic
frames; the logic stays the 60 Hz frame-synchronous machine. Code: `recomp/platform/native/
present.{h,cpp}` (the capture, the interpolator, the sprite tags), the clock in enhance.cpp (the
contract's), the played loop in runner.cpp `play()` with host_sdl.cpp `host_present_at`.

- **The clock** (R3, no added latency): runner frame k's picture R_k is drawn at its V-int and
  shown at k x T (T = 262 x 3420 master clocks, 16,688.2 us), as with the option off; the clock's
  ticks strictly between (k-1)T and kT show R_k's video state drawn again with its scroll and
  sprites moved back toward R_(k-1) by 1 - alpha. R_k exists before they are drawn (the logic of
  runner frame k is done): the frames in between only show earlier what R_k shows. Played: the
  pads are read right after the last whole frame is shown (where the 60 Hz loop reads them), the
  frame runs at once, the in-between frames and then R_k wait for their times on the present clock
  (`host_present_at`, a frame more than 4 T late moves the clock). Against the option-off played
  loop, which shows R_k as soon as it is made (~1-3 ms after the pads), the first frame showing a
  new input's effect comes at the next tick (<= 1/H s), and the whole frame at kT: the present
  clock's definition of "when the 60 Hz build shows it" (a monitor at 60 Hz with vsync shows R_k at
  the next refresh too). The headless clock (`--present-log`) is the measured one.
- **The capture** (machine.cpp, only with high_fps and a picture): `hf_display_begin` copies the
  video model as the display starts (after the V-int's DMA), `hf_display_line` keeps each line's
  VSRAM, registers and output palette as the line is drawn (after the H-ints before it: $2200's
  split rewrites reg 4, $A, $F and VSRAM; the 224-line V-scroll tables; the lift's columns),
  `hf_display_end` closes it. Two captures: R_k's and R_(k-1)'s (each runner frame's last display).
  `HF_SELFTEST=1` redraws each capture unmoved and compares it with R_k: 28,835 of 28,835 equal on
  the movie's first 12,000 frames (the raster scenes included: the capture is exact).
- **An interpolated frame**: a copy of R_k's video model, each line drawn with its H-scroll words
  (full, cell or line mode, planes A and B: the camera and g_HScrollLinesA/B) and VSRAM words
  (full, the H-int's per-line g_HIntTblBack values, the 2-cell columns) moved from R_(k-1)'s same
  line, modulo the plane's size; a step over 64 px is a cut (snaps), and so is a line whose plane
  bases, scroll mode, size or display bit differ between the two. Sprites: see below. Not moved:
  the directly written sprites (the HUD panels, the boss meter, GO!, the x 0-15 cover, the
  stage-1 fire, the stage-3 reveal), unmatched ones, the palettes and tiles (R_k's). The machine's
  VRAM, VSRAM and registers are never written (the copy is drawn and dropped); the game's state
  never sees it (E4's corpus: sync rows equal stepped-gen's at 120 and 144).
- **Sprite owners** (native-only lines in the C, R2): Obj_RunList (0087B6) names each object
  before its handler runs (`hf_spr_owner`, 0 after the list); Spr_AddSorted (key node and chained
  entry), Spr_AddBack and Spr_AddFront name each entry they write (`hf_spr_add`: the address the
  walk reads it from); Spr_BuildSAT's original walk names each node (entry, x, y, first SAT index)
  and closes (`hf_spr_walk_end`, before sprites-p6's native_table). A display's tags hold when its
  VDP table equals g_SATBuffer as the walk left it (the link bytes aside; a lag frame's V-int sends
  no new table). Each sprite moves by its node's step: the same object's node in R_(k-1) (the same
  place among its nodes, else the nearest), else where the object was drawn 2 frames back (objects
  drawn every other frame), never further (a run drawn from `--render-from N-2` must interpolate
  frame N as a run drawn throughout: the latency probe compares the two); a step over 48 px snaps.
  Counted on gameplay frames (`present.sprites`: the walk's sprites on the chain;
  `sprites_matched`: moved, or still, from their object's place). Movie, first 12,000 frames at
  144: 221,546 / 224,200 (98.8%); not matched: 1,429 front-list entries no add named that frame
  (unowned), 1,225 new objects, 0 snaps.
- **Per-line scroll counted** (`enh_scroll_frame`): an interpolated frame "uses per-line scroll"
  when its H-scroll or VSRAM words differ from line to line or its 2-cell columns differ, and
  (hfps-2, the default of the queued 4401 question) one of them differs from R_(k-1)'s on a line of
  the same geometry: a static raster or a cut shows the same picture either way. It is
  "interpolated" when the interpolator moved at least one of them (unchanged: a section that moves
  but always snaps still counts and fails).
- **Section 4401** (hfps-2): the Freeze fight's cone raster (mode $38: plane A line H-scroll + the
  H-int per-line V-scroll, Stage4_InitConeRaster) is reached by one corpus run only,
  scenario-level-skip (runner frames 11254-11834): the stepped machine's tas, scenario-ending and
  the late fuzz runs are in the attract loop long before stage 4 (sections $0001-$0005 from runner
  frame <= 100,000). There the camera is locked (Camera_LockHere) and no cone is drawn: every line
  is "no span" (plane A H 0, V = -line) and equals the previous frame's on 1,393 of the 1,394
  counted frames; the other is the cut from 4400 (registers 0, 10, 15, 16 differ). same_geometry
  snaps on no register the H-int changes there (the H-int writes VSRAM with auto-increment 0; reg 15
  and 10 are not in same_geometry). So nothing to interpolate, not a snap.
- **Capture details added by hfps-2**: each line keeps the two H-scroll words it reads as VRAM
  held them then (the lift's H-int rewrites $BC00 mid-display; widescreen's reshift_hscroll too);
  widescreen.cpp's put8 (the view's per-line VRAM writes, centre_reclaim) reports each write
  (`hf_display_vram`, a capture hook) and the redraw applies them at their lines; the R12 margin
  columns (`ws_margin_columns`, outside the GVDP struct) are kept per line and moved like the VSRAM
  columns, the last line's put back after; the bars ws_frame_check paints are painted on the frames
  in between; the tag check skips the x word with widescreen on (the render camera shifts every x
  for the frame). `HF_SELFTEST=1` on scenario-level-skip (every stage) at 144: 50,237 of 50,237
  frames redrawn equal to R_k and line by line, in 4:3 and in 16:9 (before: 4,666 in 16:9); 16:9
  tags every gameplay display as 4:3 does (11,281 / 11,281; before 8,137).
- **unlimited_sprites** (hfps-2): Spr_BuildSAT's native second walk names its pieces again
  (`hf_spr_walk2`: a listed node's entry, or the owner `spr_host_list_refused` recorded in
  SprHostNode.owner for a node the cap refused); the table rewritten in the uncapped order and the
  host table past it are tagged, and the frames in between move the host table's sprites in a copy
  (`gvdp_set_host_sat` for the draw, the machine's own table put back). Game-over snow (tas runner
  frames 10500-14000, the only refusals the stepped movie has): 497 of 497 host-table displays
  tagged, 681 frames in between moved host sprites, the selftest exact.
- **Negative controls** (hfps-2, `HF_PLANT_VDP=1 HF_PLANT_LATE=20000 phase.py 6 --draft --only E4
  --fresh`, private artifacts): E4 fails with "120 Hz: 154 runs differ from stepped-gen, first
  ('tas', (3, ['vram_crc32'])); 144 Hz: the same; whole frames: sync 20000: whole frame
  B789305B5591FA49 != --fb 360303772D72AEDA". HF_PLANT_VDP: every interpolated draw adds 1 to the
  machine's VRAM $FFFF; HF_PLANT_LATE=K: runner frame K's whole present row shows R_(K-1).
- **Lines nothing moved on** are copied from R_k's own picture instead of drawn again (exact: a
  line is copied only when its scroll words did not move, no moved sprite covers it before or
  after the move, and the renderer's carried sprite state entering it is R_k's; each line keeps
  that state, `spr_edge` and `spr_mask_carry`, in the capture). `HF_SELFTEST=1` also draws every
  frame in between line by line and compares: 28,835 of 28,835 equal. The movie's first 6,000
  frames at 144: 24.4 s -> 13.0 s (the option off: 6.1 s).
- **Measured** (`phase.py 6 --draft --only E4`, private artifacts, main 948b1d5 + kit 0.11.0,
  before the copied lines): the corpus passes at 120 Hz 737 s and 144 Hz 854 s (S3 < 900 s;
  1,105,605 runner frames in 12 processes each); their 154 sync.csv files byte-identical to the
  shared stepped-gen reference at both rates (10,175,896 rows each, checked by hand: the kit's
  comparison found no stepped-gen traces in the private dir, see below); the movie stage (144 and
  off with `--fb` and the present log) 829 s and the 10 latency probes 396 s raised nothing (whole
  frames on time, sprites, render p99, latency); the corpus tas at 144: 788,980 / 801,787 gameplay
  sprites matched (98.4%), render p99 2,567 us. Failing: "per-line scroll never interpolated in
  sections 4401" (the stage-4 H-int section: frames that use per-line scroll there are counted but
  none moved a value; not looked at yet) and the kit's "no reference ...stepped-gen\tas\sync.csv"
  (a private artifacts dir has no stepped-gen corpus: run E4 with the shared DKIT_ARTIFACTS or
  link recomp/stepped-gen; a kit point, not edited here).
- **E4 passes** (hfps-2, `phase.py 6 --draft --only E4 --fresh`, shared artifacts, kit 0.11.2,
  main 1a28113 + hfps-2's commits, 2,118 s): "2 rates x 154 runs equal stepped-gen; 130,055 whole
  frames on time; sprites 98.4% matched; render p99 1978 us; 20 scroll sections interpolated;
  latency: 10 probes at N=[424, 1934, 3432, 4938, 6445, 7952, 9472, 85871, 87394, 88886], none
  later". Uncached stage times (S3 < 900 s): pass hfps120 619 s, hfps144 486 s (before the copied
  lines: 737 / 854 s; the passes shared the machine with other workers' runners, 10 verify
  processes seen during the 144 Hz pass), the movie
  stage 421 s, the latency probes 387 s. The control run (lighter load): 444 / 574 s. With
  widescreen=16:9 (high_fps=144 against 16:9 off, the kit's whole_rows_check, scratchpad
  hf2/ws_whole.py): every whole row on time and equal to its --fb row, the movie 130,055 and
  scenario-level-skip 20,906; sprites matched 98.4% / 98.3%.

**Limits.** The capture copies VRAM once as the display starts: a game write into VRAM during the
display other than the H-scroll words a line reads (an H-int changing tiles or a name table) is
not seen by the frames in between (none in the corpus: the selftest is exact on every stage);
widescreen's own writes are seen only through put8. R11's margin pieces (ws_draw, the host's margin
sprites past the 4:3 view) carry no tags and are drawn where R_k has them. The bars snap with
R_k. Played runs at a rate the monitor doesn't match tear or judder like any timer-paced present
(no vsync).

## Phase 6: the corpus section runs (R19, R22, corpus-p6, 2026-10-30)

Before: the only corpus run the native machine plays past stage 1's street is scenario-level-skip
(the stepped TAS desyncs in $1100 and the fuzz runs branch from it), about 650 frames per section,
so E3's streamed margins rested on 4,576 frames and E4's $4401 on 1,394 frames of a locked camera.
`tools/corpus_runs.py` writes 17 scenarios `emu/corpus/scenarios/sec-*.txt` (corpus runs
`scenario-sec-*`): level-skip's own frames up to a section, then 3,600 frames of seeded fuzz-style
play (a chord held 2-40 frames, never Start; `WALK` R 0.6 L 0.08 U/D 0.12 A 0.45 B 0.15 C 0.2, A
fires). The kit branches corpus runs only from the movie's states, so the reference replays the
prefix (~20 s a run); natively each run plays from power-on in its own process (the branch plan's
only carrier is the movie). Every run is the same on both machines as far as the sections go.

- **sec-SSSS-s1 / -s2** for the eight sections whose streamed layer is valid (`layer_now()`;
  measured on level-skip's native run with `BR_WS_DUMP`: $1300 $1301 $1400 $2100 $2300 $2400 $3100
  $3200; $1100 is drawn whole, the stage-4 sections have no layer): branched at the press that
  ends the section before (level-skip would idle 500 frames there and take hits), so play starts
  with the load.
- **sec-4401-freeze**: branched 500 frames into $4000 (level-skip's `# section 16`; the hall refills
  health), the skip code three times at once (150 frames after each), then 3,600 frames of
  fighting. Level-skip's own idles leave the player 4 / 16 health and 2 lives at the fight.

Measured on the native runner (widescreen=16:9, high_fps=144, `BR_WS_DUMP` rows, before the
corpus pass; "left/right end": the map's edge within 40 px of the 4:3 view, where the render
camera shifts; arena lock: g_CameraLockX set):

| run | runner frames | gameplay frames in the section | streamed | camera x | locked | left end | right end |
|---|---|---|---|---|---|---|---|
| sec-1300-s1 / s2 | 4,753 | 3,156 / 3,495 | same | 0..1183 / 0..1211 | 0 | 31 / 304 | 0 |
| sec-1301-s1 / s2 | 5,387 | 3,582 | same | 0..637 / 0..704 | 0 / 110 | 32 / 315 | 0 |
| sec-1400-s1 / s2 | 5,983 | 3,572 | same | 0..1126 | 1,224 / 1,033 | 32 / 304 | 0 |
| sec-2100-s1 / s2 (lift, 304-px map) | 6,527 | 3,158 | same | 0 | 3,176 | 3,158 | 3,158 |
| sec-2300-s1 / s2 (flight) | 7,745 | 3,585 | same | 0 (layer 0..3648) | 3,603 | 27 | 309 |
| sec-2400-s1 / s2 (flight) | 8,359 | 3,590 | same | 0 | 3,608 | 627 | 0 |
| sec-3100-s1 / s2 | 9,524 | 3,066 | same | 0..1076 / 0..1078 | 254 / 841 | 232 / 66 | 0 |
| sec-3200-s1 / s2 | 10,708 / 10,710 | 3,385 / 3,367 | same | 0..522 | 3,022 / 2,720 | 18 / 297 | 0 |
| sec-4401-freeze | 14,086 | 3,706 in $4401 | 0 | 0 (locked) | 3,724 | - | - |

Streamed frames per run in all (the prefix's sections included): 3,156-7,294 (level-skip alone:
4,576). 132,060 more native runner frames a pass (the corpus had ~1.26 M). Not reached: the right
end of any walking map ($1300 4,064 px, $1301 2,080, $1400 4,704, $3100 6,656, $3200 4,496) and of
the $2400 flight. Random play at weapon level 0 (level-skip collects nothing) advances ~1,100 px in
3,600 frames and loses both lives; a section ends at its right end only on a real route (the TAS,
which the native machine cannot follow).

**$4401 has nothing to interpolate unless Freeze reaches 15 %** (R19's "fights with the cones"
cannot be met): the Freeze fight draws no cone (the cone owners are the hall's hover pods: the
cone paragraph above), its camera is locked, and its only scroll motion is g_ViewY's screen shake
at the 15 % energy break (Shake_Start before `Boss_Freeze_DropStep`: 8 frames of g_ViewY moving in
the TAS's whole fight, f130380-130499). The TAS gets there with weapon level 6 in 1,650 frames
(12,288 -> 1,843 energy). From level-skip's state (weapon level 0) nine fixed strategies on the
reference (fire every 18 frames from three places, charge, jump-fire, B, up-fire, dodge) did
72-400 energy in 3,000 frames, losing both lives or nearly; the corpus run does 664 (12,288 ->
11,624, reference). sec-4401-freeze natively: 8,949 per-line-scroll frames in $4401 counted at
144 Hz, 0 interpolated (level-skip's 1,394 / 0 as well), with the counting as worded.

**R19 re-ruled (the user, 2026-10-30: "Count moving + a shake run")**: 0821ff7's moving-frame
counting is back (055fea2), and the Freeze run was to reach the shake by raising the weapon level.
Not reached in the time box (queued in QUESTIONS.md, default "count moving frames only"):
- no weapon cheat: Pause_Update's only code is the section skip (Tbl_009CDC); the weapon level
  rises only through dropped items (`Item_CollectWeapon`: a value of the player's own type raises
  the level, another type switches it; the director's level-up power-ups). The TAS's levels: 1-4 in
  the street by f4049 (the trash cans at the start, f603), 5-6 in $1300 by f7846.
- at level 0 a greedy reference bot (12-frame macros, 48-frame lookahead, score = damage + health)
  did 2,360 damage in 3,000 frames and lost a life; 10,445 are needed. The same bot farming the hall
  ($4000) or the street ($1100) for 3,000-4,000 frames collected no level; scripted street runs got
  one type switch, no level.
- the native machine drifts from the reference inside the fight (sec-4401-freeze, RAM at the same
  syncs: Freeze's energy 11,864 vs 11,912 at sync 12,500, health 240 vs 224; the frame-synchronous
  machine's known lag-frame difference), so a sequence searched on the reference does not carry
  over: a shake route would have to be searched on the native runner.
With the moving counting $4401 counts 0 frames at 144 Hz, so it is absent from present.scroll
(enhance.cpp enh_scroll_frame creates a section's entry only when a frame is counted) and E4
neither fails it nor counts it among the interpolated sections (p6_enhance.py: `flat = [s for s, v
in scroll.items() if v["frames"] and not v["interpolated"]]`; the PASS line's "N scroll sections
interpolated" is len(scroll)).

## Phase 6: the crane fight's stop and the raw code scripts name (crane-crash, 2026-10-30)

**The user's crash** (the package of fb8cde7, 16:9 / 144 Hz / unlimited sprites, "as the 3d crane
was just starting to enter the fight", the window closed, no Windows error event) is the flow
runtime's fatal `[handc] flow: no C for code address $010726 (the bridge's table: 2133 entries; R8:
any other address is fatal) (C function $005438)`: a clean exit (no WER event), not an option. When
jetpack Harley's second flight ends, the claw leaves the crane (Mot_SeekScreenYOrBranch,
Op2_CruiseDown) and its coroutine 1 (`Op2_FxSpawnRegs $0480` at $1E85A, beside Shake_StartRandom)
drops debris (Op4_Spawn at $01ECDC, a4 script $01ECF2: Op4_Move, Op4_BranchTouchPlayer,
Op4_BranchCrossY $C3). A piece touching a player runs `Op4_CallNative dc.l $010726, dc.w $20, dc.l
Scr4_010964`: $010726 was raw (incrom $010724,$1E). The TAS is never touched by it, so no corpus run
reached it. Headless, on the same input, the native runner stops at sync 10861 of 10,896 with
every option set alike: cartridge 4:3 (music_voices=0), the runner's defaults, widescreen=16:9,
high_fps=144, unlimited_sprites=1, all three; stepped-gen plays through (its RAM equal to the
native's at every row before the stop). Fixed by converting it: `EnemyAtk_HitScriptSpark` (the
script's damage word as a flinch, its spark at the contact point; else it skips the 6 operand
bytes), hand C, 010726.syn (hit, already struck, invulnerable; at EnemyAtk_HitTouch's first call,
the same Op4_BranchTouchPlayer + Op4_CallNative shape). After it every option set plays all 10,896
rows, RAM equal to stepped-gen's at every row.

**The repro** (`sec-1301-crane`, `tools/corpus_runs.py` RECORDED, play in
`tools/corpus_play/sec-1301-crane.txt`): level-skip to 500 frames into $1301, then 9,728 frames found
on the native runner: a closed-loop driver (each step replays the whole input from power-on, reads
the RAM at its end; ~2,000 runner frames a second) beats the first jetpack Harley, then a greedy
search (40-48 random chord sequences of 96-160 frames per round, one process each, 8 at a time,
scored by the boss's lost energy, the player's health and closeness) beats the second; the claw
comes down at runner frame ~10,750, its debris hits at 10,861 (the stop), and the input goes on
~1,100 frames into the claw fight. The native runner frames to the stop are
`tools/corpus_play/sec-1301-crane.native.input` (`--benchmark 10896 --input-script`). As a corpus run
the scenario drives the native machine along the found path only to runner frame 7,478 (Harley's
second flight): see below. Lessons from it: Batman's B is the jump (held up to ~20 frames
for height), A the attack; with level-skip's weapon level 0 a hit takes $20 of the jets' $250, and
the jets hover at y $40-$50, reachable only from the crates; the claw enters only when the
population is 0 (`Op2_BranchPopulationNz` at $1E96A), i.e. after Harley's second flight. The
reference (BizHawk) plays the same pads but its game parts from the native's in the first jetpack
fight, then lags, fades and catches up. The stepped input takes the pad of each D1 sync row's
reference frame (VPADs for the frames between), so the play file has a padding frame on every
reference frame with no sync row (fitted on the real D1 trace of a scratch artifacts dir, since a
private dir refuses a timed trace into the shared traces: 2 runs; the lag column alone is not
enough, 2,338 lag-free frames of level-skip have no sync row). That makes the corpus's native input
the driven frames 1:1 until the reference's first catch-up after a lag burst, where it takes two
sync rows in one frame (runner frames 7,479/7,480 share reference frame 7,935, and so on every 3
frames for a while): two runner frames get the same pad, which no padding changes. From there the
corpus run's native game parts from the found path (it loses Harley's second fight, game over at
~12,000). A scenario that reaches the claw through the corpus needs a search that keeps the pad
across those pairs, or the reference's catch-up points as a constraint. The native runner's
`--branch-plan` branches did not play their own pads here (two branches, L and R, ended at the same
x), so the search ran one process per candidate.

**Raw code that script operands name** (beyond ws-tail's spawner-stub scan): a scan of every
native-calling op outside the function spans (Op2_CallNative/CallNativeSwitch/CallNativeOnChild,
Op4/Op5/Op6_CallNative, Op2_CallNativeRel/RelSaved/RelSwitch, Op2_StartCoroutine2Rel, inline
Op2_Native/Op4_Native) whose target is neither an instruction start nor inside a function span, the
coroutine scripts (first word an op handler) dropped. Stages 1-2, converted with hand C and a
synthetic case each: $00924A `Arc_HeadAtMark` (a4 native: heading at g_ArcTarget, stage 2 spawn
scripts), $013540 `Item_CollectSmallHealth` (+$10), $013F92 `WeaponDrop_FromTopRandom`,
$01DCFE `Boss_JokerBalloon_ThrowByArg` / $01DE6A `Boss_JokerBalloon_SpawnThrown` (the balloon boss's
11 thrown objects, Tbl_01DD48 / Tbl_01DE8A), $020316 `Stage1_LaunchBalloonC` (the third height
table Tbl_020388), $025450 `Enemy_Barrel_KnockByHit` (inline), $02787A / $027896
`Enemy_LadderThug_SpawnD/E` (Op2_CallNativeRelSaved x7 each). Stage 3-4 hits went to raw-stubs:
$03B79E-$03B7BC (Op2_CallNativeRel), $042420, $042484, $04291A, $0467C0, $04689E, $0468F4, $049236
(inline natives). ws-tail's stage 1 and 2 stubs (39) are converted the same way: the 12 stage 1
spawn-action items and trigger spawners (spawn actions 3-6, 9, 10, 13, 20, 33-35: the level-down
weapon pickup `Item_SpawnWeaponDown*` (value 0, table $13C5C), `Item_SpawnWeapon0/1`,
`...Weapon1FromHit`, `...Weapon2FromHit`, `...WeaponRandomOrDownDrifting`, the small health pickup
`Item_SpawnSmallHealth*`; `Enemy_Clown_SpawnRanges`) and stage 2's 27 (pole thug E, the missile
launcher jet B, three missiles, 20 black/red drone courses, dome craft C, the green nav light
flipped). A ROM slip found on the way: spawn action 33 (`Item_SpawnSmallHealth`) goes to the large
health pickup's body ($1341E), so that drop gives +$80, not +$10. Each new function's contract is
its sibling's (the same callers) or measured (`disasm regs`), its synthetic case at a sibling's
first call that the stepped corpus reaches (verify: every case passes, every instruction covered).
**Coordination**: a new function changes every reference run's function list, so `corpus status`
turns them all stale; until the shared corpus is rerun, the gate and verify ran with
`corpus.status` taking runs that differ only in the function list (read-only on the shared dir).

## Phase 6: the native watchdog (stall-p6, 2026-10-30)

**The stops.** Five native branch processes stopped on "[stepped] no sync point for 30 s after
sync N: stuck; last game function entered $00A400" (kit-combo's 144 Hz pass and its ws0 and ws2
passes, ws-centre's 16:9 pass and one of its 3 replays): fuzz-01800-s1 after 1698, fuzz-07200-s1
after 7063 (twice), fuzz-86400-s1 after 86033, fuzz-90000-s1 after 89603. Not a hang and not the
restore:
- **Every fuzz-*-s1 branch has one 1,779-V-int sync interval**, 29-30 runner frames after its
  branch point: the script's PADS $7FF (every button, Start among them) then 1,778 VPAD lines.
  The game pauses and Pause_Update waits V-int by V-int at its IRQ_WAIT sites, so the whole pause
  is one sync interval (scene $9D72 throughout). Same points, same signature as the stepped
  machines' stops of 2026-10-27/28 (stepped-gen-branches/fuzz-*-s1 at syncs 1698, 3496, 5296, 7063,
  8858, 86033, 87803, 89603), which were real and are fixed. fuzz-01800-s1's case was the process's
  first branch: a save, no restore before it.
- **The state at each stop is a passing run's.** Row 7063 (and 1698) is byte-identical, every
  column, to that row in the passes that played on (ws-ctl, ws-host, hfps-p6, the shared dirs).
- **The interval takes ~1-3 s of CPU alone** (rendered 144 Hz, fuzz-01800-s1: DKIT_STEPPED_HANG_S=1
  stops at 1698, =2 plays through; 3,490 rows in 10-15 s). The watchdog counted wall seconds
  without a sync point, so a process that got no CPU for ~30 s inside that interval stopped.
- **Runners here do go that long without CPU.** Every job runs at below-normal priority; a
  1-s monitor of every runner process (CPU time per process, the machine's busy %) saw, while the
  machine sat at 100% from 21:22 to 21:26 with work that was not ours, 10 other workers'
  below-normal runners go 32-98 s with no CPU (the 11 together got 0.8-3 CPU s/s). The watchdog
  thread wakes once a second, gets the wait-completion boost and keeps counting; the CPU-bound
  game thread does not run. One rendered fuzz-01800-s1 replay of mine stopped the same way at
  1698 with the process's CPU flat for 23 of its last 30 s.
- **Controlled**: suspending only the game thread (the watchdog's left running) for 35 s at its
  2nd CPU second stops the old runner "after sync 570 ... entered $009BF0": any stall of 30 s
  stops it, anywhere; the pause interval just makes a much shorter one enough.

**The fix (runner.cpp, machine.cpp, input_script.cpp).** The watchdog measures game progress
against the game thread's own CPU time:
- progress is `nm_progress()`: sync points plus IRQ_WAIT V-ints (each one a reference frame
  played), counted outside the snapshot so a restore never looks like no progress;
- the clock is the game thread's CPU seconds (GetThreadTimes / pthread_getcpuclockid): stuck when
  the game thread has spent DKIT_STEPPED_HANG_S (30) CPU seconds with neither. A starved process
  spends none; a game spinning on a RAM flag spends them at full speed (as before, ~30 s);
- a pause nothing ends would now count as progress for ever, so nm_wait_vint bounds a sync
  interval by its input: no more V-ints than its VPAD lines plus the reference frames the script
  still holds (`ns_peek_frames`, ns_peek's walk counted; one hour of game time without a script).
  Past that the run has left its reference, and it stops at once, naming the interval;
- a game thread that is blocked rather than spinning (waiting in the host) is reported after 20 x
  hang seconds of wall time without progress.
Same rows: fuzz-01800-s1's sync.csv with the fix and DKIT_STEPPED_HANG_S=1 is byte-identical to the
old runner's passing run.

**Controls** (exe built from this tree, private dir): `BR_PLANT_HANG=1000` (the game thread spins
at sync 1000, no V-int, no sync point) stops in 31.6 s wall / 31.1 s CPU: "no sync point or V-int
for 30 s of the game thread's CPU (30 s) after sync 1000". fuzz-01800-s1 cut after its first 2
pause VPADs (the pause never ends) stops at once: "sync interval after sync 1698 took V-int 3, past
the 2 reference frames its input has" (the old runner: its hardware-read guard, 20,000,001 reads,
6.5 s). The 35-s game-thread freeze: the old runner stops at sync 570; the fixed one plays all
3,490 rows. A 25-s freeze at CPU 4.5 s: both play through (it missed the pause).

**Replays under load** (tas6-to-32085's whole plan, 37 branches, 16:9 with --game-rows
--hash-centre, 6 old and 6 fixed processes at once, 21:47-22:00 while the machine sat at 100%
with 38 below-normal runners getting 3-28 CPU s/s between them): **old 2 of 6 stopped** (both
"after sync 3496", fuzz-03600-s1's pause, 0.2 s of CPU in their last 30 s), **fixed 0 of 6**,
though its processes went as long without CPU (25-26 s flat stretches, the same as the old ones'
33 s). All 10 finished processes' sync.csv are byte-identical (old and fixed alike). ws-centre's
replays of the same plan had stopped 1 in 3. Whole passes with the fix (private dir, fresh):
`phase.py 5 --only P1 P2 P7` 3/3; native 16:9 (--game-rows --hash-centre) 171/171, 0 stopped,
1,049 s alongside the P5 job; 144 Hz rendered 171/171, 0 stopped, 560 s.

**Harness lesson** (the measurement's own stall): a runner whose stdout is a pipe nobody reads
blocks in printf once its GENESISRECOMP_BRANCH lines fill the buffer: 12 of my replays froze at
sync 5288 with 0 CPU and the main thread in branch_tick -> ucrtbase -> WriteFile (stack walk). The
kit's subprocess.run drains it; a hand harness must send stdout to a file.

**Not changed:** the stepped runners' watchdog (the kit template's spec.c) still counts wall
seconds without a sync point. Their passes are unrendered (the pause interval well under a
second), and they have the hardware-read guard; none has stopped falsely so far.

## Phase 6: the weapon-level poke and sec-4401-freeze-lv6 (R24, 2026-11-17)

**`@poke ADDR VALUE`** (kit 0.11.9): a scenario line that writes a word to work RAM between two
frames. The reference writes it, trace.json records it (`"pokes": [[frame, addr, word]]`), the
timed runner's .input gets `WRITE_RAM16` after the WAITs that add up to that frame, the stepped
runners' at the start of the sync interval holding it (sync k in a frame <= t, sync k+1 after),
and the native runner applies WRITE_RAM8/16/32 as the engine's runner does (input_script.cpp,
through dk_mem.h at the word's width). A poke run plays alone, never as a branch (the branch seek
refuses a script that writes RAM). **Exactness** (scenario-sec-4401-freeze-lv6, `@poke $F616 6`
after frame 363): the timed runner equals the reference on all 18,991 frames; frame 363's RAM row
is the unpoked sec-4401-freeze's, frame 364's differs (921d1bea against fa5f6e6a) on both. Sync
342 is in frame 363: on the timed runner, stepped-gen and native it equals the unpoked run's
(2f917d5a), and sync 343 holds the same new CRC on all three (e1b13cb4 against 89f34934).
stepped-gen == native on all 16,196 rows; timed and stepped-gen part at sync 459 (frame 536),
as the unpoked control does (the frame-synchronous machine's known difference).

**g_StartWeaponLvl** ($F616): Main_Start writes 0 at frame 3, the player's spawn ($101A8) copies
it into the record at frame 583, once a run (func_frames of level-skip and sec-4401-freeze); a
poke between them gives level 6 through every later section. A lost life drops the level by 3
(6 -> 3 -> 1 measured in $4401).

**Level 6 is not the TAS's whole advantage.** At the spawn-aligned frame (TAS f128916 = ours
13279) Freeze's objects ($AE08, $B31A, $B374) equal the TAS's but for his energy, and the TAS's
player record has +$26 = $180: the rapid-charge timer (`Item_CollectRapidCharge`, picked up in the
hall), so its shots are charged. Replaying the TAS's fight inputs from frame 13140 with a test
poke of +$26 ($F676 = $20B, the TAS's timer at that frame) follows the TAS exactly (same player
x/y, Freeze's energy 12,208 / 11,248 / 10,528 ... as the TAS's) until a 1-px subpixel difference
(+$40) adds up and our player dies near 13,700; without the timer the same inputs do a third of
the damage. Seeded play (FIGHT, seed 1, 3,600 frames) did 2,300 damage and lost a life.

**A route found on the reference does not carry over to the frame-synchronous machine.** A greedy
reference bot (12-frame chords, 72-frame lookahead, damage less health; ffb3e96's play file) got
Freeze to 15 % (1,834 of 12,288) at frame 18,787, `Shake_Start` 18,863, `Boss_Freeze_DropStep`
18,863-18,910 (48 frames, as the TAS's f130401-130448), no life lost. On the native runner the
same pads (one sync point a frame in the fight, one lag frame) part within 500 frames (Freeze
10,726 against 10,700 at frame 13,740), the player loses both lives, Freeze ends at 8,192.

**The run** (E4 measures the native runner, so its fight was searched there;
tools/corpus_play/sec-4401-freeze-lv6.txt's header says how): a greedy bot on the native runner,
each candidate a native replay from power-on with RAM dumped at its last sync (31 at a time,
~9 s a 12-frame step), scored as the reference bot plus the distance from where the reference's
hits landed (level with Freeze, 50-150 px to his side: without it the bot idled 600 frames at
10,230). The chords map one reference frame per runner frame (the play starts a frame early,
PLAYED), corrected once through the run's own D1 sync frames (one lag frame at play frame 4,297):
the stepped input then equals the searched one on all 5,243 compared runner frames. **Native
(== stepped-gen, all 15,728 rows): 15 % (1,794) at sync 15,472, g_ShakeY 4 6 6 4 . . 2 3 1 at syncs
15,532-15,540 (g_ViewY with it), one life lost (level 6 -> 3).** The reference's own play of these
pads parts from the native's (10,230 against 10,490 at frame 14,340) and ends at 8,508, level 1:
the reference run does not reach the break; the timed runner equals it on all 18,523 frames.
**E4** (merged with R23/R25, 2026-11-17): PASS, 21 scroll sections interpolated, "reached with
per-line scroll and no motion: none"; $4401 in this run: 18 moving frames, 12 interpolated at
144 Hz (16 / 8 at 120 Hz), the shake. sec-4401-freeze and level-skip still count 0 there.

## Phase 6: the table cases left raw (fix-spawn-table-gaps, 2026-11-17)

**The stop.** The user's playtest of the package (16:9-gameplay, 144 Hz, unlimited sprites) ended
right after the crane boss: `[handc] flow: no C for code address $013060 (C function $012ED0)`.
SpawnAction_Run's handler table Tbl_012EF0 (50 longs, itself an `incrom` span inside the
SpawnAction_Run file's neighbourhood) named two raw handlers: entry 40 ($13060, nine A health
pickups) and entry 48 ($12FE4, the random A pickup picker). Their headers already said "never run";
the earlier scans (crane-crash, raw-stubs) read stub shapes and raw-data longs whose neighbours
were function starts, and missed this table's two.

**Two sweeps over the whole ROM** (scratchpad scripts ptr_sweep.py, jt_sweep.py):
- *Pointer tables*: every aligned long whose value is an even raw address, with at least two
  distinct neighbours (+-4, +-8) that are bridge entries at $1000 or above. 75 hits; by hand all but
  $12FE4 and $13060 are script words or data (a 0 then -20 reading as $0000FFEC, $02231C a data
  pattern a Hatter script names, graphics).
- *PC-relative bra tables* (`jmp`/`jsr d8(pc,Xn)` followed by bra.s / bra.w cases): 19 raw cases in
  6 tables, and Drop_RandomHealthA's own 4 at $13006. Each table's size checked against its index:
  - Op2_BranchTouchBox ($374A) kind $C, Hitbox_TestAttackBoxAt ($4E22) kind $C (and its handler
    $4E4A), Hitbox_LinkByKind ($4F3E) kinds 4 and $C: box kinds no corpus object has;
  - Drop_RandomHealthB ($13030) cases 2 and 6: the TAS's 26 runs drew only 0 and 4 (its C jumped
    straight to B2/B3, so the native port was safe there; the generated code was not);
  - VInt_BusyScrollTbl ($AEC4): offsets 4 and $3C are modes the stage-4 scenes set ($03BB88), so a
    busy V-int there would have stopped; $14, $18, $1C, $24, $30 are modes nothing sets;
  - VInt_ScrollModeTbl ($AE7C) $14-$24: **not converted**. Every g_ScrollMode store is an
    immediate (4 8 $C $10 $28 $2C $34 $38 $3C), so those slots are dead; mode $20 alone would pull
    in 465 instructions of raw handler.
  Also seen, not chased: the raw span $FFC2+$148 inside 00FE0E's file (an unreached branch of a
  converted function).

**Converted** (20 function files, 1317 -> 1337): `Drop_HealthA9`, `Drop_RandomHealthA` and its
`_case0/2/4/6`, `Drop_RandomHealthB_case2/6`, `Op2_BranchTouchBox_KindC`, `Hitbox_KindAllEnemies`,
`Hitbox_HitAllEnemyBoxes`, `Hitbox_LinkByKind_AllPlayers/AllEnemies`, `VInt_BusyMode04/14/18/1C/24/30/3C`.
Each has hand C on a sibling's contract (Drop_RandomHealthA, Hitbox_KindAllEnemies,
Hitbox_HitAllEnemyBoxes and Op2_BranchTouchBox_KindC take the sibling's measured prototype: the
same code) and a synthetic case at the sibling's first corpus call. Gate (private dir, runs taking
the 1317-function list): lint 1473 files, ledger 1337, entries ok, native and D2 tas 6000 syncs
equal; verify over 173 runs: every new function checked (31-218 synthetic calls), 0 mismatches,
every instruction covered; Drop_RandomHealthB's 2,669 real calls equal.

## Phase 6: finding the ways into code without C (R27, raw-reach-check, 2026-11-18)

After the crane-boss stop the user asked for a better way than playtests to find the code the port
has no C for (P5 R8: any other address is fatal). Three parts (R27):

**The static check, E9** (`uv run tools/raw_reach.py`, ~40 s). What can send the game to an address:
- *fixed branches* (bcc/bra/bsr/jsr/jmp with a known target) in converted code: none land in raw
  bytes or on another function's non-entry instruction;
- *pc-relative bra tables*: the six dead scroll-mode cases, listed in config/raw_reach_allow.txt;
- *scripts*: the VMs are threaded code (each op ends `movea.w (aN)+,a0 / jmp (a0)`), so a script word
  is an op address. tools/script_walk.py walks every `Scr<N>_` span and every op position the TAS ran,
  running each op's 68000 code abstractly at that script position (script values known, RAM not;
  an unknown branch forks; loops on unknown data that move the script register stop the path as
  "opaque"), with the op lengths the TAS showed where every observation agrees;
- *script-named calls*: ops that call a long or a relative address from the script
  (Op2_CallNative*, `movea.l (a2)+,a0 / jsr (a0)`): every target must be a table entry.

**Ground truth for the walk** (`Observed`): the reference emulator keeps, per ROM byte, the first
instruction that read it (the harness's `dk_reader`); replaying the TAS (130,772 frames, 71 s) and
keeping that map gives 19,153 script positions read first by a dispatch: proven op positions, and
605 of 641 ops with one operand length. Without it the abstract run alone lost most scripts: yields
(an op that saves a2 in the object and pops another script) read as ends, and count-prefixed lists
ran away. A position the corpus READ is never a dispatch to a raw op (that dispatch would have run
the op, and every op the corpus ran has C), so those are walker errors, not findings.

**What it found:** six raw ops named at op positions no corpus run reached, converted with hand C
(1337 -> 1343 functions): Op2_SetTimer1Rand ($0F88, Scr2_0392A0), Op2_BranchVar1Ne/Gt ($2130/$2140,
Scr2_0386CC), Op2_ApplyMoveBranchAtY / Op2_ApplyMoveLandAtY ($4C5E/$4C9A, Scr2_01DEB6) and
Op4_ForkAndEnd ($60BC, Scr4_036F88). Listed, not converted: $07F6, an ILLEGAL instruction three
scripts install as the handler of a vector that never fires (`3AC2 00DC 07F6`): on the cartridge it
takes the illegal-instruction vector, $0440800C, outside the ROM, so the original stops there too.

**Its limits** (printed by every run): 28 ops whose effect the walk can't read (indirect jumps
through object data, the bit switches such as Op2_SwitchVar2Bits) stop their
paths; 219 positions are lost (a word that is no op address where the walk expected one: mostly
data blocks after a script, some walker errors). The walk does not prove the scripts behind those;
the corpus and part 3 cover them.

**Synthetic cases need the sibling's operand count.** The first case for Op2_SetTimer1Rand ran at
Op2_SetTimer1's call (one operand word): the new op read two, the ops after it ran on misaligned
words into garbage ("dispatch miss: $000001") and the two sides differed far downstream. A case
runs every op after it until the VM yields, so it runs at an op with the same operand words
(Op2_SetTimer1RandYield1's for SetTimer1Rand, whose next op follows a yield: Op2_SetSpeedRand's
continuation never yielded in 21 runs and the verify stopped those runs early).

**The crash report** (recomp/platform/native/crash_report.cpp): a played session keeps its pads per
displayed frame in the --input-script format (PADS / WAIT, the F9/F10 changes as OPTION lines, which
the input script now applies); dk::fatal (its new g_fatal_hook), a host exception or the machine
ending writes crash-reports/<time>/report.txt and inputs.input beside the runner and shows a message
box. BR_PLAY_SCRIPT=F plays a script through the played loop and BR_CRASH_TEST=N stops at frame N:
the log's round trip is checked with them.

**The boss runs** (part 3; tools/corpus_runs.py BOSS_RUNS, emu/corpus/scenarios/boss*.txt): the TAS
beats each of its 12 boss fights once (boss meter g_BossMeterObj at TAS frames 4540, 12000, 18920,
32140, 45130, 70460, 72930, 75370, 77190, 87530, 100740, 113320), with one random state; the crane
boss's death chose spawn action 40 only in a playtest. Each boss run starts 300 frames before a
fight, sets g_BossEnergyScale / g_EnemyEnergyScale ($F64C/$F64A, $100 = x1.0, applied by the a2 op
$39AC as an enemy spawns) to 1 and plays 6,000 frames of seeded FIGHT chords, 3 seeds: on the
reference 11 of 12 bosses die within the 6,000 frames (boss 7, $72930, survives seed 1), and every
enemy dies in a hit or two, so deaths and drops run many times with other random states. The 36
runs (8 s each on the reference) executed no code without C after this round's conversions. (A
level-skip start reaches no boss in 3,600 frames: the bosses end their sections. `emu.poke_ram`
writes one byte; a scenario's `@poke` writes the word.)

**After the 209-run records (2026-11-19):** the boss runs read 268 bytes of the stage 2 boss's script
no run had (labelled Scr4_027B1C, bd8d535); a `@poke` landing inside an open shadow test made 6
calls incomparable (verify.cpp step 2b now skips them, bd8d535); the two ApplyMove ops needed a case
per path for D4 (cac30c6); S3's 900 s gate is now exceeded (QUESTIONS 2026-11-19).
For the kit (`/kit-harvest`): p6_enhance's E1 content key omits `recomp/game/verify/` (an edited .syn
returned a stale cached E1: run `phase.py 6 --fresh`); the verify template's step 2b; and
tools/script_walk.py + raw_reach.py (the reader-map ground truth) as a general static check.

**S3 after the records (2026-11-19):** the passes were over 900 s for two reasons, both fixed with
the corpus and the gate unchanged: the native runner's per-frame hashes (a rect test per pixel, a
byte-at-a-time CRC; ea730fc, values identical) and Windows packing the 8 runners onto 5 physical
cores (kit 0.11.10 spreads them, 5cde577). Fresh record b030f15: 202-633 s a pass. For the kit:
`genesis/verify.py`'s runner launch has no `spread=True` yet (E1's verify pass ran 4 runners two to
a core, 2026-11-19); the reference corpus's processes neither.

## The front door (the X-Men 2 harvest, item 3, front-door, 2026-11-20)

The user, 2026-11-20: "proceed through all of those items autonomously until complete" (item 3 of
docs/progress/xmen2-harvest.md: the kit's playbook/porting.md "The player's front door"). All in
`recomp/platform/native`, adapted from the X-Men 2 port's; the game's state, traces and sync rows
are unchanged with every option at its default (the runner before and after: tas 30,000 frames,
scenario-level-skip 23,997, scenario-two-player: equal sync rows and per-frame hashes).

- **The ROM by its bytes** (runner.cpp `rom_check`): each form a dump comes in is tried and the one
  whose SHA-1 is the cartridge's taken: plain (.md/.bin/.gen), a 512-byte copier header, SMD
  interleave (16 KB blocks, odd bytes' 8 KB first; with or without the 512-byte header), MGD (all
  odd bytes, then all even). Files other than 2 MB or 2 MB + 512 are not read past 2 MB + 16 KB.
  The refusal keeps "SHA-1" and exit 2 (the kit's package smoke reads both).
- **No ROM given**: settings.ini `[rom] path` (the one chosen last), then any 2 MB / 2 MB + 512 file
  beside the exe that is the game (names sorted: the same choice each time), then, played, a file
  picker (Windows' open dialog; Linux: zenity, then kdialog) until a file that is the game or
  cancel; the pick is written to `[rom]`. `--benchmark` runs never ask.
- **The console**: a played run alone on its console (`GetConsoleProcessList` = 1: a double-click)
  calls `FreeConsole`; a terminal or a pipe keeps its output; a harness run started with
  CREATE_NO_WINDOW has no console window to close.
- **Message boxes** (host_sdl.cpp `host_message`): a refused or missing ROM and a crash report say so
  in a box on played runs. Printed instead (`[native] message: ...`) under `--no-messagebox`,
  `BR_NO_MESSAGEBOX=1`, the kit's harness environment `GENESIS_NO_LAUNCHER=1` (every kit runner launch
  sets it: recomp.py, package.py, the export's verify.py) or `SDL_VIDEODRIVER=dummy|offscreen`; no
  picker opens there either. On Windows SDL shows real boxes even with the dummy driver
  (environment.md), so this is what keeps a headless run from waiting on the desktop. Lesson
  (2026-11-20, this worker): running the bare exe with no arguments to see its usage opened the
  launcher on the user's desktop (stopped at once): every manual run of the runner here sets
  `SDL_VIDEODRIVER=dummy GENESIS_NO_LAUNCHER=1 BR_NO_MESSAGEBOX=1`.
- **The launcher** (launcher.cpp, Dear ImGui 1.92.9b vendored in `external/imgui`, fonts Karla and
  Roboto in `external/fonts`, built into the exe by native.cmake): tabs Play (the ROM with Browse and
  its verification, what differs from the cartridge, player 1's controls), Options (every option of
  options.cpp's table, grouped Picture / Sound / Gameplay with labels and help; one the launcher has
  no entry for shows under "Other" with the table's own text), Controls. Shown on a played start with
  no ROM, no `--input-script`, no `BR_PLAY_SCRIPT` and no `GENESIS_NO_LAUNCHER`, unless `[launcher]
  show = 0` (its checkbox) or `--no-launcher`; `--launcher` forces it. PLAY writes `[options]`,
  `[player1]`/`[player2]`, `[launcher]`, `[rom]` (every other section, the kit's `[input.pN]`, kept as
  written) and hands the game the ROM, as a command line would. Its own window and renderer, per-monitor
  DPI aware on the thread while it runs (SetThreadDpiAwarenessContext, fonts scaled by DPI / 96: 2.5 on
  this machine), closed with SDL_QuitSubSystem before the game's window opens DPI-unaware as before.
  Headless test: `BR_LAUNCHER_TEST=script` (commands in launcher.cpp's header) on a hidden window with
  the dummy driver, `BR_LAUNCHER_DRYRUN=1` prints the plan instead of playing. Theme: see "The
  launcher's theme" below.

## The launcher's theme: Midnight (launcher-themes, 2026-11-20)

The user chose it from screenshots ("go with midnight+"), after three rounds. Round 1 offered 11
candidates: 5 moods (the first theme, Signal, plus dusk, batcave, joker and a light newsprint one) and 6
built only from Genesis colours taken from game scenes (the title, the night city, the airship, the
forest, the red tunnel, the checkerboard room). Round 2 made readability variants of the night city.
Round 3 went between Signal and City Slate. Midnight sits halfway: Signal's blue-grey buttons and
yellow on Slate's near-black, with navy outlines brightened in its "+" form. WCAG contrast on the page:
text 17.4:1, help text 8.8 (Signal 6.7), yellow 12.1, outlines 3.5 (Signal 2.5), an empty box against
the page 1.5 (Signal 1.36).

- A ticked checkbox has its own ImGui colour (`ImGuiCol_CheckboxSelectedBg`, new in 1.92), which the
  first theme never set: ticked boxes showed ImGui's stock blue. Now it is the field colour, and so
  are the table header and the scrollbar's hover and active colours.
- Under `BR_LAUNCHER_TEST` the window is no longer shrunk to the screen's usable size. A test's shots
  are the same on every machine, including at `BR_LAUNCHER_SCALE=2` (2200x1520) on a smaller screen.
- How the candidates were judged, for the next port: render every candidate with the test mode (a
  script of `tab`/`focus`/`shot`), lay all of them out in one image per page, and give the WCAG
  ratios of text, help text, outlines and empty boxes. A phone downscales a stack of 7 pages until
  it can't be read: render at 1.5-2x and lay them out two per row.
- **Controls** (controls.cpp): 2 players, the 3-button pad's 8 buttons (the game reads a 3-button
  pad: `Pad_ReadPort`), each two keys and two SDL game-controller inputs, a controller per player
  (plug-in order, a pinned GUID, none), keyboard layouts (arrows Z X C Enter; W A S D J K L Space),
  dead zone (37% by default: the former 12000 of 32767 was 36.6%, so a stick between 12000 and 12123
  now counts one step later). Face buttons by position (`SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS=0`:
  a Switch pad's A/B were by label before); an unknown joystick is mapped in the DirectInput order;
  a hot-plug shows `PLAYER N: name` on the picture. The defaults are the old fixed mapping.
  `BR_PAD_SELFTEST=1`: 17 steps with SDL's virtual controllers (slots, remapping, dead zone, pinning,
  a generic joystick, the settings text).
- **Level select, `start_stage`** (options.cpp `k_start_sections`, dk_opts.h `OPT_START_SECTION`,
  Main_Start's new game). The game has no start-scene switch (its aids: the pause code that ends a
  section, the dead debug pause menu $1569C), but the section order is clean: Main_NewGame sets
  g_SectionId = 0 and Section_RunNext steps (from, to) pairs of Tbl_009FFC: 0 -> $0001 -> $1100 ->
  $1300 -> $1301 -> $1400 -> $0002 -> $2100 -> $2200 -> $2300 -> $2400 -> $2401 -> $0003 -> $3100 ->
  $3101 -> $3200 -> $3300 -> $3400 -> $3401 -> $0004 -> $4000 -> $4300 -> $4400 -> $4401 (then the
  ending). Each section is a routine that sets up its whole scene (Tbl_00A0AC), and each but $4401 is
  the continue table's (Tbl_00A05C) restart point ($4401 restarts at $4400). The option sets the new
  game's g_SectionId to the section before the chosen one. Values: `0`, `2`/`3`/`4` (a stage's
  opening), `1-street 1-gem-expo 1-warehouse 1-parade 2-lift 2-flight 2-clouds 2-airship 2-beam-ship
  3-backlot 3-cat-head 3-mushrooms 3-spotted-room 3-card-hall 3-hatter 4-hall 4-lab 4-tube 4-freeze`.
  Every value runs 5,200 frames from a new game to the end with g_SectionId the chosen section.
- **Linux**: the same sources (ImGui needs only SDL2); see the package result in
  docs/progress/xmen2-harvest.md row 3.

## Phase 6: the X-Men 2 harvest applied (R28-R30, xmen2-harvest, 2026-11-20)

Kit 0.11.11 harvested X-Men 2's four weeks after release; the user had this port catch up ("proceed
through all of those items autonomously until complete"; docs/progress/xmen2-harvest.md).

- **high_fps's frames in between, evenly (R28).** runner.cpp's played loop and enhance.cpp's
  present clock put the ticks of an absolute H Hz clock between whole frames, with the whole frame
  on top: at 144 Hz 1 or 2 frames a span (E4's report under kit 0.11.12: even in 0 of 130,054
  spans). `enh_between_times` cuts each span (k-1)T..kT in n = H x T rounded parts (120 and 144: 2,
  165: 3, 240: 4); the movie's first 3,000 frames at 144: 2,999 of 2,999 spans even. X-Men 2's same
  clock gave a 120 Hz display one dropped picture in three ("seemingly random slowdown").
- **The sprite table's copy (R29).** The engine's renderer reads y, size and link from the VDP's
  own copy (`GVDP.sat`, written by `gvdp_sat_byte`), x and the pattern from VRAM. present.cpp's
  moved sprites and widescreen.cpp's `put8` write both (X-Men 2's wrote VRAM only for four weeks).
  `enh_sat_begin/line/end` check the copy against VRAM's table on every drawn line, whole frames
  (machine.cpp fsync_display) and frames in between (present.cpp draw), as `sat_checks` /
  `sat_cache_mismatch`. Movie, first 3,000 frames: 0 mismatches with high_fps=144, both 16:9 modes,
  16:9 + unlimited_sprites and 16:9 + 144; `BR_SAT_SELFTEST=1` (present.cpp skips the copy) 2,555
  mismatched renders of 5,998 and another present log.
- **The presentation reference (R30).** E10 (kit 0.11.13 `pres_ref`): the movie's present log with
  high_fps=144, widescreen=16:9, 16:9-gameplay, unlimited_sprites=1 + 16:9 and high_fps=144 + 16:9,
  digests per 1,000 rows in docs/presentation_ref.json; `uv run tools/phase.py
  accept-presentation --reason "..."` after a meant change.
- **The capture path** (kit playbook/release-video.md: footage is recorded inputs, replayed at any
  setting). `--record-inputs F` (played) writes the session's pads at exit as an input script with
  its replay line (crash_report.cpp's log, the F9/F10 changes as OPTION lines). A replay renders it:
  `BR_CAPTURE=F.mkv` (capture.cpp: every runner frame, FFV1 through ffmpeg, 400 x 224 with
  widescreen else 320 x 224, narrower pictures centred) and `BR_WAV=PREFIX` with `--audio-hash`
  (sound2.cpp: PREFIX.mix/.music/.sfx.wav, the music and effects machines apart with
  music_voices on).
- **Release habits**: docs/RELEASING.md (the recipe from a fresh detached worktree, the notes'
  shape, the asset round trip); `tools/traffic_archive.py OWNER/REPO` (views, clones, referrers,
  paths, stars, downloads into artifacts/traffic/, merged daily: GitHub keeps 14 days); the
  package's own licenses and README (dkit.toml [package], kit 0.11.13); docs/public/README.md's
  numbers rechecked (1,343 functions, 209 runs and 12.7 M sync points, 289 unnamed data spans).

## Quick saves and rewind (R33, rewind, 2026-11-25)

The user's pick of xmen2's three player features (P6-enhance.md "Rulings of 2026-11-25"), on the kit's
playbook/porting.md "Quick saves and rewind" and xmen2's `platform/native/rewind.cpp`.

- **Keys.** F5 saves to the slot in use, F7 loads it, F6 picks the next of 4 (on-screen notes as F9/F10's:
  `SAVED TO SLOT 2`, `SLOT 3 IS EMPTY`, `<< REWIND`, `REWIND: NO OLDER FRAMES`). Rewind held: the Controls
  table's 9th row (`CB_REWIND`, `[playerN] rewind = ...`, no pad bit, either player's), by default
  Backspace (player 1's keys; the WASD layout has none) and every controller's left trigger (the game uses
  neither trigger). F5-F7 join Esc and F9-F11 as keys no button may take; a settings.ini from before whose
  button already has Backspace or a left trigger keeps it (Rewind's goes). The launcher's Play page lists
  them. settings.ini `[rewind]`: `seconds = 60` (0-3600; 0 = no ring, the slots still work), `sound =
  reversed` (default, the user's) or `silent`. Not an option (`--options --json` unchanged, E2 and S2 as
  they were): a played run's feature, nothing on disk (a state holds the game fiber's raw stack).
- **States** (state.cpp, machine.cpp `nm_state_init`): the machine as named sections into caller-owned
  snapshots, beside the branch plans' one-at-a-time copy (which stays as it was): `fiber` (the game's C
  frames and run loops: 3,336 B at a sync point), `arena` (work RAM's values and store tags, 131,072),
  `flow` (the flow runtime's registers and loop pointers into a buffer: flow.cpp `state_save`, 112),
  `machine` (g_machine, host pointers scrubbed, 91,672), `platform` (MState, the sound's frame number, and
  unlike a branch the Z80's EI shadow and instruction time, 176), `sprites` (spr_host, 9,236), `ws_draw`
  (9,396), `widescreen` (WsSaved, 2,716), `sound` (12,167: sound2.cpp `snd_state_save`, machine 2 and the
  views as a branch keeps them plus what a branch restarts from the registers: each ymfm chip's own state
  (`save_restore`), its carried writes, output filter and pending samples, the held events, the engine's
  YM2612, PSG and event queue). 259,883 B a state. The digest (`state_digest`): FNV-1a over 8-byte words of
  every section but the fiber's stack. Every saved struct's padding is zeroed or copied as it lies (a
  stack copy with garbage padding would make equal states differ).
- **The ring** (rewind.cpp): at each host frame's end the state is saved and the ring keeps, for frame k,
  S_k -> S_(k-1) per section (XOR with the newer, whole when its length changed: the fiber's live stack)
  as zero/literal varint runs, the input frame k began with and S_k's digest; the newest state whole. A
  step from C decodes S_(C-1) and S_(C-2), loads S_(C-2) and the host loop runs C-1 again with its input,
  drawn and shown; at its end the state's digest must equal the one kept (`rewind.mismatch`). Held at the
  oldest frame the picture stays and the game waits. The input kept per frame (runner.cpp): played, the
  pads, where the session's input log stood (crash_report.cpp: a step takes the log back there, a slot
  carries a copy of its own, and options changed since are written after it, so a crash report's replay
  still reaches its frame) and with `BR_PLAY_SCRIPT` the script's place; headless, the input script's
  place (`ns_position_save/load`: its VPAD queue and what `ns_peek` reads ahead are part of the frame).
- **Presentation and sound after a load.** The high_fps clock counts host frames (runner.cpp play()), not
  the machine's frame number, so a load moves nothing on it (xmen2's present clock was frame-numbered and
  needed `present_jump`); a load drops the interpolator's previous frame (`hf_invalidate` after every load),
  so the next frame is shown whole. A quick load and a rewind's start drop the device's queued sound.
  `[rewind] sound = reversed`: host_sdl.cpp takes the replayed frame's FM to the PSG rate with
  audio_flush's own index (fi = i * fm_n / psg_n), reverses FM and PSG, and holds it one frame so the last
  one before forward play (or the held picture's silence, or a load) fades out; the first sound after fades
  in (an eighth of a frame). The kit's sound.md: reversing the FM at its own rate would pair other samples.
- **Tests** (env, no option): `BR_REWIND_KEYS=STEP:save|load|slotN|rewindN,...` at the STEP-th host frame end
  (counting the rewind's own and held pictures'); headless runs record only with it (`--branch-plan` and it
  are refused together). `BR_REWIND_SKIP=SECTION` leaves a section out of every load; `BR_REWIND_SECONDS`,
  `BR_REWIND_SOUND`; `BR_REWIND_SELFTEST=1` (the encoding, 400 checks); `BR_DEVICE_WAV=F` (host_sdl.cpp: the
  engine's tap of the stream sent to the device). Counters: the exit line's `[dkit] enhance` gains
  `"rewind"` (saves, loads, empty_loads, steps, holds, mismatch, bad, the ring's frames, bytes, bytes per
  frame, push time, the state's size) and `"state_sections"` whenever the feature is on; played runs now
  print `[dkit] options` and `[dkit] enhance` at exit too (a player's log shows them).
- **tools/rewindcheck.py** (headless, the movie): plain vs keys, the kit's "Equal rows don't count plays":
  every sync row and `[FBHASH]` frame hash equal to the plain run's for its sync point and frame, and each
  frame written exactly as often as the keys play it (simulated from the keys alone), plus mismatch 0 and
  the step/hold/save/load counts. 6,000 frames, cartridge options: `rewind300` (300 held at steps 1,500
  and 3,600: 5,700 plays, 602 frames more than once), `saveload` (back 400, forward again, slot 2),
  `slots` (an empty slot twice), `ring2s` (a 2 s ring, a 300 rewind: 119 steps, 181 held pictures) all
  pass; also with `widescreen=16:9-gameplay,unlimited_sprites=1,high_fps=144` and at the movie's first
  sprite-list overflow (`--frames 12500 --opts unlimited_sprites=1 --cases drops`: frames ~11,000-13,000,
  482 frames of list drops by 12,000). The ring recording with no key changes no row or hash.
- **Each section left out of the loads** (`--controls`, cases rewind300 + saveload): arena, flow, machine,
  platform, sound caught at the cartridge's options (digests, rows, hashes); ws_draw and widescreen caught
  with 16:9-gameplay (empty in 4:3: not caught there); sprites caught only where the list overflows with
  unlimited_sprites=1 (it holds nothing else: not caught elsewhere); fiber not caught headless (every
  headless frame ends at the sync point, Frame_End's entry, with the same frames on the stack) and caught
  in played runs (tools/rewindsound.py: a played frame can end at a V-int inside the game's code on a
  stack of its own: over the boot and title the run dies).
- **tools/rewindsound.py** (played, dummy video and audio, the movie's pads per host frame, the device's
  WAV tap and `GENESIS_AUDIO_FLUSHLOG` for the frame boundaries): rewinds of 900 (boot and title) and 120
  (play) over 2,400 frames: 1,020 replayed frames each the plain run's same frame backwards sample for
  sample outside the fades, the 3,420 forward frames equal; `silent` zeros; no setting = reversed; the
  sound section left out: caught (its replays sound different). Pacing with high_fps=144: 2,040 more host
  frames took +34.3 s (34.0 meant).
- **Cost** (the movie's first 6,000 frames, headless, cartridge options): the ring 3,394 B a frame (60 s =
  3,600 frames = 12.7 MB), recording 168 us a frame (the save, the XOR runs, the digest), the run 1.62 ->
  1.82 ms a frame with every frame drawn and hashed; 2,917 B a frame over 12,500 frames (10.3 MB a
  minute); 209 us a frame with 16:9-gameplay + unlimited + 144. Measured with the runs in parallel.
- **Linux** (the package's GCC build, under WSL): the selftest 400/400; the movie's first 4,000 frames with a
  300 rewind, a save and a load: mismatch 0, every sync row and frame hash one of the plain run's, 5,000
  written; 3,638 B and 267 us a frame. Both packages build and pass their smoke and ROM-window checks.
- **Not checked by these**: a person's keys and a real controller's left trigger (the pad selftest's
  virtual one only: 19/19), how the reversed sound sounds through speakers, an F9/F10 pressed during a
  rewind (the replay then runs with the options as they are now and may count mismatches), an input
  script's OPTION or WRITE_RAM lines rewound past (the script's place goes back, an option set or a poke
  already made doesn't).

## Phase 6: the played window, scaling and shader presets (R32, R34, present-modes, 2026-11-25)

Both from the X-Men 2 port (`platform/native/host_sdl.cpp`, `shader.cpp`), as the kit's porting.md
"Scaling modes, with a key to switch" and "Shader presets" describe them. The window's alone: the
game's frame, every trace, digest, present log and E10's pictures are as before (no gate sees the
window), so `tools/present_check.py` reads the played window back and checks it.

- **`scaling`** (options.cpp; R32, default `sharp_bilinear`, its cartridge setting for E2 since it is
  presentation only): host_sdl.cpp `draw()` fits the picture (s = min(window / picture)); `integer`
  uses floor(s), centred, black around it; `sharp_bilinear` between whole multiples draws the frame
  nearest to k = floor(s) times into a target texture (`s_pre`, remade on a size change, freed with the
  renderer) and that, linear, into the fitted rectangle: each game pixel k screen pixels plus at most
  one blended. At a whole multiple (the 3x window) and without render targets it is nearest, so the
  default 3x window is the same in all three. **F4** steps sharp_bilinear -> integer -> nearest with a
  message ("SCALING: EVEN PIXELS / WHOLE NUMBER / SHARP"); F4 and F8 are reserved from binding.
- **`shader_preset`** (shader.cpp; R34, default "" = none): a RetroArch `.slangp` through librashader
  0.12.0, the library loaded at run time from beside the game (`librashader.dll`: the upstream zip
  fetched by native.cmake by SHA-256; `librashader.so`: built from the tag's source, commit
  `87e8a97b`, `--locked`, by `tools/librashader_linux.py`, which native.cmake runs at configure when
  the per-version cache `~/.cache/batmanrobin/librashader-0.12.0/` lacks it; RUNPATH `$ORIGIN`). D3D11:
  the chain in its own `ID3DDeviceContextState` (single-threaded, as SDL's device), the back buffer's
  RTV taken after a black fill + `SDL_RenderFlush`. OpenGL (Linux: SDL's `opengl` renderer is asked for
  when a preset is set, 3.3 needed): the chain draws into an output texture, blitted to framebuffer 0
  turned over, SDL's cached GL state saved and put back (program, VAO, array buffer, textures and
  samplers on units 0-2, framebuffers, viewport, scissor, blend, masks, unpack row length). No library,
  a preset that doesn't load or compile, 3 failed frames: one `[native] shader_preset: ...` line and
  the plain picture. **F8** turns it off and on; the hotkeys' message is a layer drawn over the shaded
  picture (unshaded). With a preset, `integer` sets the rectangle the preset draws into; the other two
  give it the fitted one. No preset bundled; player-facing text names none (RELEASING "No taste picks").
- **The log:** a played run's exit prints `[native] picture {"renderer", "window", "fullscreen",
  "scaling", "scaling_keys", "shader", "shader_frames"}` beside `[dkit] options` (which now lists
  both options) and `[native] shader: N frames through the chain, GPU time per frame p50/p99/max`.
- **The ROM-window scan:** `librashader.dll` (SHA-1 `b70fc9f6...`) and the Linux `librashader.so`
  (SHA-1 `9c6538c7...`, the same bytes as the X-Men 2 port's build of the same commit: the build
  reproduces) hold **no** 256-byte window of this ROM: X-Men 2's near-run ($08 then 356 zeros) is its
  ROM's, not this one's. So nothing is pinned for them (P5's "Third-party files" stays SDL2.dll
  alone) and no slack is added.
- **The Linux zip ships `librashader.so`** (fixed 2026-11-26 by kit 0.11.17): `package_linux` (dkit
  0.11.16) staged the runner alone, where `package_windows` copies every `.dll` beside the exe; it
  now copies every `.so` the build put beside the runner. The package: 17 files (`batmanrobin`,
  `librashader.so`, README.txt, settings.ini, LICENSES/ with `librashader-MPL-2.0.md`), 0 ROM
  windows, smoke and movie trace exit 0; the runner's RUNPATH is `$ORIGIN`.
- **Checked** (`uv run tools/present_check.py`, Windows, Direct3D 11; the movie's inputs played in a
  real window, `BR_WINDOW_SIZE`, `BR_BACKBUFFER_SHOT` at picture 1500, stage 1's street with its
  dithered fog; F4 through the event queue, `BR_HOTKEY_TEST`). Windows (Direct3D 11): 35 of 35 cases.
  - Scaling, each mode against a model drawn from the frame read beside the window (numpy): `integer`
    exact at 960x672 (3x), 1920x1080, 1280x800, 1024x768, and 16:9 at 1920x1080 and 1280x800;
    `sharp_bilinear` within 2 levels (max 1.24-1.63); `nearest` exact but where the GPU picks the
    neighbouring texel at a boundary (636-7,230 pixels, every one the neighbour's colour: whole rows
    or columns at a seam); at 3x the three identical to the model (max 0); the exit line after two F4:
    `nearest`, 2 presses.
  - Shader presets (presets written by the check: a one-pass identity `nearest.slangp`, a two-pass
    `blend.slangp` of our own with linear sampling; 1440x810, 3.6x): `nearest.slangp` equals the plain
    window pixel for pixel under sharp_bilinear (= plain nearest), integer (= plain integer), nearest,
    at 3x and in 16:9; a preset naming a missing file, one that doesn't compile and no
    `librashader.dll` beside the game each equal the plain picture pixel for pixel, the log naming
    why; `blend.slangp` differs from plain (905,084 pixels), F8 off gives the plain sharp_bilinear model
    within 1.56 levels (SDL's state intact after the chain, the render-target path included) and F8
    on the shaded picture again under the message. GPU time per frame (D3D11, nearest.slangp): p50
    6-10 us, p99 12-391 us.
  - Linux (`--linux --bdir` the WSL build dir; WSLg, Mesa's OpenGL 4.5 over D3D12; SDL's GL renderer
    plain, the `opengl` one with a preset): 35 of 35. Scaling as on Windows (integer exact;
    sharp_bilinear max 1.17-1.31; nearest seams only, 0-2,693 pixels). `nearest.slangp` equals the
    plain window exactly at 3x and under integer; at a fitted size (nearest and sharp_bilinear, and
    16:9) the two windows differ at seams only (1,452 and 1,557 pixels: both are the nearest model but
    for seam texels): SDL samples its 512-wide texture and the chain its own 320- or 400-wide one, and
    Mesa rounds a seam differently for the two (D3D11 doesn't). The check reports these as
    `seams_only` rather than exact; an orientation or placement error is never a seam. Broken
    presets and no `librashader.so` (the runner alone in a folder: RUNPATH `$ORIGIN` finds nothing)
    equal plain exactly; F8 as on Windows.
  - Linux vs Windows (`--compare-linux`): `blend.slangp` within 2 levels (shaded, F8 off, F8 on), plain
    sharp_bilinear within 2, integer exact, nearest windows seams only: 11 of 11.
  - Not checked: a real 1080p fullscreen display, a played session by eye, a Steam Deck, a real Linux
    GPU driver (WSLg's GL is fine for correctness, no measure of cost), a real slang-shaders preset (the
    check's presets are its own).

## Open questions

What was tried, so the next session doesn't repeat it.

- **P4 D4: the 12 undetermined files as synthetic cases (cov17, 2026-10-29; ruling 5 step 2, each
  case's state consistent with the entries below, its comment citing them, 0 mismatches on the
  runs named).** Answered: Op6_CallNativeRetry's park (0070BE.syn clouds_on, at Flight_FrameClouds);
  the cross-table equal handles (006834.syn shared_rom_new / shared_ram_new, 007EF6.syn: the Hatter
  road bank's $FEB6 and ROM $000A, both on $8080); the refcount $80 (006834.syn users_128,
  007F5E.syn users_128, 0085B0.syn users_128); Op4_DrawFrame's descriptor-short and cached-release
  paths (006834.syn no_descriptor, release_cached); BossMeter_Show with a live meter (011ACA.syn
  meter_live: at Op2_SetBossEnergy's first call the balloon's meter IS live, so only the second Show
  is synthetic) and ArtCache_Acquire's in-use path (0085B0.syn in_use); VIntObj_RunList with more
  than 3 objects (008ADA.syn eight_more: 8 VIntObj_NewIdle objects linked by hand, scratchpad
  cov17_vint.py); the hull's -500 clamp (026070.syn); Stage3_DrawRoad's clamps (0406CC.syn);
  Layer_QueueColumn's left wraps (012A84.syn, at Flight_FrameAirship on g_LayerA: a left step
  outside the flight would contradict P4-decomp.md's ScrollStepX row, so not at ScrollStepX's own
  first call, which is in stage 1) and ScrollStepY's -16 clamp (012B74.syn, at
  Boss_ForestRobot_Spawn); Coll_MoveBlockMap's ceiling hits (011D5E.syn, the truck map's own
  ceilings, scratchpad cov17_ceil.py); Coll_WalkBlockMap's stops (0120A2.syn: walkers off a floor at
  the GemExpo map's own walls, scratchpad cov17_walls.py; past the map's right end; below the map).
  Verify runs from power-on through the TAS to a fuzz run's branch, so a case's `at` state is the
  TAS's (fuzz-48600-s1 = TAS to f48600 + 3600 fuzz frames). Measured (`tools/phase.py 4 --only D4`
  on 71d5f40, private artifacts): D4 1,144/1,145, 0 mismatches. **Left: Coll_WalkBlockMapLeft's block
  reload at an in-map block edge ($012432-$012446)**: it needs a 2-px wall in column 15 of a block
  with a map column to its right; no intact block map has one (cov13; scratchpad cov17_col15.py /
  cov17_col15all.py: 0 in 6,563 RAM dumps of 24 runs whose g_TerrainFn is $010DE2 / $010DF0, or the
  lift while Stage2_MoveOnScrollingMap runs). The only RAM states with such walls are stale level
  data: the truck after the Joker balloon's loader Scr6_01FE8A decompresses over g_SceneBuf
  (TAS f18900 on, ~500 such blocks; g_TerrainFn is already Coll_FlatFloor $010E20 by then) and the
  arena's stale lift layer (flat floor too). A row needs the order of that overwrite against the
  terrain switch (if the switch is never later, no state runs it); a case would have to build the
  garbage-collision state, which no finding makes consistent with a block-map walker. Not done.
  The truck's order is fixed: the Joker balloon boss script runs `2CBC 0001 0E08 00B6`
  (Op2_CallNative Coll_ScriptSetFlatFloor $010E08, floor $B6: TAS f18878) and then `43E2 0001
  FE8A` (Op2_StartA6Script, the loader) at $01C4FA, so the overwrite always follows the switch
  there. Stage 2: the lift's terrain is the flat floor, its block-map walkers are the barrel thugs'
  Stage2_MoveOnScrollingMap (TAS f22331-f31862), and the lift layer's data is overwritten by
  f32100 when the arena's terrain $0247C6 is installed ($02417E); a row still needs that no barrel
  thug walks after that overwrite (thug survival into the arena not checked).
  **Answered (cov18, 2026-10-29): a synthetic case, not a row** (0120A2.syn left_wall_in_map_edge:
  a grounded walker at GemExpo (2560, 167) walking left into column 15 of the platform's left end,
  block (159,10), its collision data $FF281E given a 00-0F column 15; 0 mismatches on fuzz-07200-s1,
  $012432-$012446 covered). The proof does not hold: a thug CAN survive into the arena. The thug
  body ($024CC8) only dies off-screen by Op2_KillOffscreen100 ($0008CE: screen y outside
  [-100, 324)) at its loop top ($024D0C), and the lift map's top girder is at map y 258 (block row
  16, cells spanning x 57-247, scratchpad cov18_top.py on an idle run's dump), so a thug standing
  there has screen y ~233 at the top and lives on. The trigger table Tbl_02105A (scratchpad
  cov18_trig.py) spawns thugs at screen y $10 up to y key $F3 (two Enemy_BarrelThug_SpawnA at x
  $78/$C8); Trigger_RunY fires a key once the scroll drops below it and, when the spawner returns
  d0 != 0 (Obj_SpawnEnemy: pool short), retries every frame until the key passes scroll+$E0. On
  time the feet ($19 below y) start at map y ~284 and fall past the girder (an idle run from TAS
  f31500, cov18-idle31500: thug moves end f31947, arrival f32065, Coll_WalkBlockMap never run);
  a spawn delayed to scroll <= 217 lands on it and walks on the arena's garbage after the
  overwrite. Whether the pool is ever short there, and what the garbage holds, is not checked.

- **P4 D4 after the stepped level-skip (cov16, 2026-10-29; whole-corpus verify on b249c6a, kit
  0.9.15, stepped-gen coverage of 154 runs from private artifacts, which now work for COV3_SG).**
  (Answered by cov17's synthetic cases, the entry above; $012432-$012446 by cov18's.) The stepped machine's new reach covered none of the 15 non-V-int files: 006776, 0070A8, 0074A8,
  007EF6, 007F5E, 008586, 008ADA, 009D24, 011A1C, 011D5E, 0120A2, 012A84, 02604C, 033274, 04062A.
  Done: 009D24 (009D96.syn both_out: the last player dying as the section clears), 0074A8 and
  033274 (rows). New leads:
  **Spr_ChangeFrame's cross-table equal handles** ($007F42 / $007F5A, and Op4_DrawFrame's $006854 /
  $006886): a RAM frame's handle is $8002 + the bank's handle offset, and of the 22 Op6_InitBankFrames
  loads (every one at FC40; scratchpad cov16_banks.py decompresses each bank) only the Hatter road's
  ($04056C, block $0A4BB0, 68 frames) reaches $8080: its frames FEB6 / FEC0 / FECA / FED4 / FEDE have
  handles $8080-$8088, drawn only by the road script's Op2_DrawSpr sites $03D708-$03D77A (a death
  animation after CallNative $03D8D8). ROM frames with those handles (scratchpad cov16_shared.py):
  $000A-$00A0 ($8080, the hex digits of the dead Debug_DrawHexBytes), $00AA-$0104 ($8082: the dead
  free camera's $AA / $B4 among them), $010E / $0118 / $0122 ($8084-$8088, read only in menu and
  game-over runs: cov16_frread.py); no ROM handle below $8080 but frame 0's $8000 in the first 900
  entries (cov16_frames.py; past $1E27E6 the 10-byte stride runs into other data, not checked). A
  row needs every live source of ROM frames $000A-$0122 (script operands of every draw op variant,
  code tables like $01EBCE / $03DF0A / $041F0E, HUD_UpdateGoSign's base + d0) kept out of the road.
  **Op6_CallNativeRetry's park via Stage3_SpawnRevealDirector**: its loader is the second a6
  script $0400B8 (DecompressBg x2, SetBufPtrToEntry, StoreLzEnd, then the native), started by the
  forest's VM5 spawn entry `E070 0000 7092 0004 00B8` at $035596 (VM6_StartFromTrigger), spawned by
  `6FB2 001C` right after Op5_WaitPopulation0 at $035574; Obj_SpawnBoss needs 4 free blocks, so the
  park needs the pool within 3 blocks of full with the counted population 0 (players, effects,
  items and shots only): not bounded. **BossMeter_Show with a live meter**: 12 Show sites (11 read by
  reference runs; $03B330 by none) and 11 Defeated sites, each read by the TAS or a scenario
  (scratchpad cov16_longs.py 11ACA 11B14); which Defeated closes which Show not traced.
  ArtCache_Acquire's only caller is BossMeter_Init (handle $ED82), so its in-use path ($0085F0,
  $0085F4, $0085F6) waits on the same question, and $0085F2 additionally on 128 users.
  **VM6_SetupMapBlit**'s tools (cov16_mapops.py: every map-draw op site and its operands,
  cov16_hdrs.py: each scene start's header and setup routine, cov16_mapw.py: every LZ block's entry
  0 read as a map) also list the scene loaders for other a6 questions.

- **P4 D4 below $00A43C after cov12** (2026-10-28; whole-corpus verify on 1f548da's code, kit
  0.9.13, stepped-gen coverage from the shared artifacts: cov3_d4.py needs the shared
  DKIT_ARTIFACTS for COV3_SG, a private one gives 0 stepped-gen runs). Done: rows for Op5_StartGroup's
  crash path (ruling 9) and Scroll_FillBands' band skip (proof: the view y stays 0..18 in every
  scene with a camera band list); cases for a bank loaded over held art (007EB8.syn, 0086AA.syn:
  the truck's and the Joker balloon's banks share frame offset FC40 and handle $8002, which the TAS
  holds with two users at f13620-f13980) and for the release that then finds the art unused
  (008000.syn, 007F5E.syn release_cached). Left: **Spr_SwapFrameArt $007FE6** and **ArtCache_Acquire
  $0085F2**: an acquire at refcount $01-$80; a positive byte is cleared by the release that makes it,
  so only $80 (128 counted users of one art) reaches them; not bounded (a bound needs the most
  frame words that can name one art: sprite blocks of the $8C-block pool plus HUD and weapon slots).
  **ArtCache_Acquire's in-use path** ($0085F0/$0085F4): BossMeter_Show with a live meter
  (g_BossMeterObj set: $011ADC, cov13's file) restarts Scr2_011B3E and Init acquires again; the cat
  head's script has two Show calls ($03B300 after a $F00 wait, $03B32E after Op2_GotoVector0):
  whether the second can come while the first meter lives is the lead, for both files.
  **Spr_ChangeFrame $007F42 / $007F5A** and **Op4_DrawKillSidesBelowOrClear**: a ROM frame and a RAM
  frame sharing a handle, not checked against Tbl_1DDB60. **SceneCb_WaitPlayersExit $009DA8**:
  both players out of play during an exit walk, i.e. the last life lost while walking off (only
  Player_DespawnNative clears +$25 bit 0; the callback is installed only with a player in play);
  no corpus exit (264 dump pairs, scratchpad cov12_exithit.py) shows health or lives falling, and
  whether the walk-off script can be hit is not traced. **Op6_CallNativeRetry's park**: the
  clouds start at TAS f46290 (Flight_StartClouds, the $2300 loader), pass at f49950, and the
  $2400 loader with Flight_WaitCloudsPassed is spawned from Scr5_021D3A after a wave Gosub and
  Op5_WaitPopulation0 at f67260: a 17,000-frame margin in the TAS; a row needs the waves' least
  duration (their waits and spawn intervals) against the clouds' 3,660 frames. VM6_SetupMapBlit,
  VIntObj_RunList: as cov10 left them. Scripts: cov12_viewy.py (view y per section),
  cov12_refs.py (data longs / relative words naming an address), cov12_shake.py, cov12_bankh.py
  (a run's RAM frame entries and their descriptors), cov12_h8002.py, cov12_frm.py, cov12_chk.py
  (a verify log's result and coverage for some pcs), cov12_grpops.py.
- **P4 D4 at $00A43C and above after cov13** (2026-10-28; whole-corpus verify on 1f548da +
  stepped-gen coverage of the shared artifacts: 9 files with uncovered instructions, 2 of them the
  V-int waits of ruling 7). Done: Boss_Harley_StepPole's 130 px cap (01A1C4.syn: a kill in the
  last 5 frames of a pole rise to $78 leaves the rising velocity under the death script's
  SetPoleTarget($3C, $1800); the TAS's StepPole runs f5683-6092, pole block $FFC488; scratchpad
  cov13_pole.py traces it, cov13_polesim.py steps a kill frame) and an Unreachable row for
  Layer_QueueColumn's 9 second-wrap instructions. Left, with what is known:
  **Layer_QueueColumn** $012A8E / $012ACE (the left wraps of the block column at x 16 and of the
  plane cell column, +$32 = 0 before a left step; the plane is $80 cells in the flight, so x in
  688..703 + 1024k there: TAS $32 vs layer x) and Layer_ScrollStepY's -16 clamp $012B88/$012B8C.
  Only the flight scrolls left (ScrollStepX's left branch runs only in fuzz-68400..77400 and the
  TAS; cov11's ScrollStepX row lists the +4 writers): the layer follows the path object of
  Scr2_02F644, whose director coroutine $030EEE seeks the targets -$64, $8C, $307, $2FD (the only
  leftward target step), $3E8, $532, $640, $898, $938, ... $BB8, $D84, $FA0 and at each reached
  target starts an X oscillator (`SetOscX 0300 00AA 00AA 00A0` at $02F690, reset to `0000 ...`
  for the seek at $02F67C); TAS swings around a target are about +-22 px ($51C-$547 at $532).
  A proof needs the oscillator's exact range (Op2_SetOscX's operands) and the path-to-layer
  offset; not done. +6 (pending y) writers: Camera_TrackPlayersXLayerA, Flight_FrameAirship
  ($96 + g_ViewY), Flight_ScrollCloudBank, Stage2_UpdateScroll (lift) and
  Attract_SkyscraperScroll: a bound on each scene's view-y change a frame would settle the clamp.
  What bounds it: g_CameraY's writers are Camera_TrackPlayersY (only the flight loop $33274; at
  most 4 px a frame), Scene_InitStage's header, Layer_MoveYWithCamera (scene setup) and
  Attract_SkyscraperScroll (the attract camera: at most -11 px a frame from power-on, scratchpad
  cov13_dmax.py over a per-frame run). g_ShakeY >= 0 falls at most 9 px a frame on its own, but
  every Shake_Start* clears it (Shake_Arm) and its first step is v0 - pull (Big 9, Start 4, Long
  4.5, Random 1-3; Shake_Rumble kicks without the clear, arena only), so one frame can drop it by
  the peak (Big 18, Long 9, Start 6, Random 4) minus that step: 17 for a Shake_StartRandom of 3 px
  on a big shake's peak frame. Big-shake sites ($95EC): $020D52 street, $02429A / $026B50 /
  $026C4A arena, $028EA4 / $0295FC stage-2 flight pod and beam ship, $036352 stage-3 rooms,
  $03C772 / $03CC02 / $03CC64 forest, $03D188 road, $0422FC stage 4; unconditional
  Shake_StartRandom ($9604): $019A5A, $01D95A, $01E062, $01E832, $01E88C, $0254AC, $0488D4. Of
  the layer-scrolling scenes only the forest has big shakes (its other starters: Long $03C7B6 and
  the generic Shake_Start ones, drop <= 14, camera fixed): a per-scene list of starters would
  make the -16 clamp a row; the scene of each script site is inferred from the runs reading it.
  **Coll_MoveBlockMap / Coll_WalkBlockMap**: the maps (RAM dumps: TAS f6600 GemExpo 254x15, 9900
  Warehouse 130x15, 13500 Truck 294x15, 79500 Backlot 416x15, 90600 Mushroom Forest 281x14,
  21600 lift 19x415; block cell >> 1 indexes +$14, the collision block is a mask word and 16
  [top, bottom] column spans, FF00 = empty) have 1-px platforms (rows 0, 7, 8, 9, 14) and 1-px
  wide walls (000F / 070F spans). Scratchpad cov13_blocks.py (cells, --walls), cov13_geo.py
  (one-step walks from grounded pixels), cov13_geo2.py (position-free conditions): no grounded
  walker meets a wall with its feet at row 0 of a block in any map (WalkBlockMapRight's $01229E..
  / Left's $012454..), and the walk-left wall stop at a block edge ($012426..) needs a 2-px span
  in column 15 of a block, which exists only at GemExpo's last block column (x 4063, y 0-63, the
  map's right end, reachable from the right only from x >= 4064, outside the map). Walls that
  span a block-row boundary do exist (the Truck's, the lift's), so the feet-row-0 paths rest on
  where walkers can stand (a walker's feet are on a floor pixel after every walk or landing;
  spawns and carried objects not checked). Coll_MoveBlockMap's uncovered $012054-$01208A: a
  ceiling hit moving up (mask bit 0 only) at a block-row boundary, or with x steps left (d1 >=
  0): its ceiling hit ran once (TAS), never at a boundary; not judged. Not written as rows.
  **BossMeter_Show with a live meter**: g_BossMeterObj survives no scene change (Scene_InitCommon
  clears $FF8000-$FFF5F8; the continue / game-over runs show it 0 at ~f42000); 12 Show sites,
  11 Defeated; the cat head's second Show $03B32E follows Op2_GotoVector0 $03B32C and no offset
  word in $030000-$048000 points at it; the flight director ($030FB8 / $030FC6) waits for the
  meter to appear and then to go before its next target. **Boss_Stage2_MoveHullRunTurrets'
  -500 clamp**: the floor roll speed (g_SceneVars2+$12) is 0..3.0 (Stage2_UpdateScroll ramps it
  by $2000 a frame during a tilt, clears it at the end), so the hull only rises while the floor
  turns, by 3/4 of the roll; the hull's own seek (StepVelocity toward target y -$28: cruise $AA
  in phase 1, $855 after Boss_Stage2_FloorTurning) brings it back. A phase-1 tilt (a bomb's
  Stage2_ArenaRequestTilt, scenario-continue f34600) lifts it ~90 px (-$89 at f34680) and the
  return takes ~180 frames; phase 2's chained tilts (every 105 frames, TAS f33656-35416, the boss
  already at energy 0 from f33605) stay within -$6B..+$2F. -500 needs about five phase-1 tilts in
  a row (a bomb landing within a few frames of each tilt's end); not settled. Two dodge probes
  from TAS f32400 (8000 frames of L/R runs, with and without jumps: scratchpad cov13_mkdodge.py)
  got exactly the idle run's single tilt at f34570 (hull -$9D at f34650) and no other: the
  phase-1 tilts look scheduled by the attack script (coroutine 0 at $026D8C: timers $1E0, $780,
  $21C0, ...), not by where the bombs fall; its schedule would bound the gap between tilts. **04062A** (the road
  clamps: pitch $0406D8/$0406E8, bend $0406FE/$04070E): player 2 can't join on the road (the meter
  script's HUD_LockJoin: BOSS TIME); joined in the card hall the TAS desyncs (the hall waits for
  its enemies); two players from power-on through the level-skip code (scratchpad cov13_merge.py
  builds it from scenario-level-skip) reach the road at f11060 and lose all lives by f15500,
  reading its scripts only to $03E54F (cov11's one-player TAS probe: $03EBB1), so the pattern's
  progress depends on more than survival time.
- **P4 D4 at $00A43C and above after cov11** (2026-10-28; whole-corpus verify on eec812f, kit
  0.9.12, + stepped-gen coverage, scratchpad cov3_d4.py with COV3_SG=1 and MIN $A43C): 19 files
  with uncovered instructions, every one never executed by any reference run (cov3_exec.py) but
  the two V-int waits (Stage1_RunTruck 31/37, Flight_RunScene 27/35: the queued question). Done:
  Span_DrawTriangle's boundary lines (00F976.syn), Weapon_KeepShotArtOne's second change
  (0109A0.syn), BossMeter_Defeated with no meter (011B14.syn: a failed meter spawn on a full pool;
  Obj_Spawn's failure `rts` $008F1C runs in the TAS and fuzz-73800-s2), and an Unreachable row
  for Coll_Stage2TiltFloorRest's no-mask path (the players' terrain-state chains, walked in the
  row), and for Span_BuildCircle / Span_DrawDiscDither's negative radius (the beam loop clamps
  the radius object's x before the copy, g_CameraX stays 0 in the flight scene, and the death
  disc's 59 moving frames start at x 20 with +5 px/f that braking $1500 a frame cannot reverse)
  and for Layer_MoveX / Layer_MoveY (every call is a fixed move from a just-initialised layer:
  the raw a6 op entries $7366/$736C/$7374/$73A6-$73BC sharing Op6_MoveLayerAToCamera's tail are
  in no a6 script) and Layer_ScrollStepX's +-16 clamps (g_LayerA's pending x is that frame's
  camera move, at most 4 px; the five layer-stepping scene headers start the camera at x 0:
  scratchpad cov11_hdrcam.py reads a Scene_InitStage header's terrain and camera words).
  Left, with what is known: **BossMeter_Show with a live meter** ($011ADC-$011AEC): the
  flight boss Scr2_030000 has Op2_KillOffScrX ($030118, screen x outside -40..700) in its main
  loop, which would remove it without BossMeter_Defeated and leave its meter for the next Show;
  whether it ever leaves the screen is not traced (two probes from TAS f70400, no input and Left
  held, scratchpad cov11_flyboss*.txt: the boss of f70500 stays until the game over at ~f74300,
  no second Show). The second Show at $03B330 (after
  Op2_GotoVector0 $03B32C) has no static reference (no Goto, no vector op, no long). TAS meter
  lifetimes (g_BossMeterObj $ED8A, 60-frame dumps): the exit after a defeat ends within 180
  frames; no two fights closer than 1680 frames. **Boss_Harley_StepPole's $82 cap** ($01A1EC): every target operand of
  Boss_Harley_SetPoleTarget is $14, $46, $78 or $3C (its 10 Op2_CallNativeRel sites $019490-
  $019A10), and a rise toward a target is clamped at it; but SetPoleTarget does not reset the
  velocity, so a lower target set while the pole still rises fast lets it coast up by v^2/2a
  (from $46 to $78 at $1000: up to ~50 px over $78 = past $82). Whether the script can set the
  next target within the ~40-frame rise depends on the vector-2 routines between the sites; not
  walked. **Boss_Stage2_MoveHullRunTurrets' -500 clamp** ($026086): the hull's picture y
  ($F5B8) starts at -300 (Stage2_LiftArriveTop) but comes down to about -45 within 180 frames
  (TAS 32100-32280); the TAS's floor turns (from f33540) lift it only to about -93. The tilt
  requests come from bombs landing (an a4 object in Scr4_026A46: Arc_Step, Op4_TerrainMove,
  then Op4_Gosub $026B46: sound, flash, Shake_StartBig, Stage2_ArenaRequestTilt), thrown by the
  turrets the hull's attack script commands (Boss_Stage2_SetTurret*Coroutine); not settled.
  **Layer_QueueColumn** (13): its uncovered paths are the wraps of a column or plane cell
  column below 0 (a left-edge column: d0 = -1, d2 = -2 from Layer_ScrollStepX's negative
  pending), which only Flight_FrameAirship gives (the airship path moving left: its targets go
  $0307 -> $02FD at $030F0C / $030F24, plus its X oscillator), and $012B88.. / $012BE8.. /
  $012C2A.. not read. **Coll_MoveBlockMap / Coll_WalkBlockMap**: the scenes on the block map
  (terrain $010DE2 / $010DF0) are GemExpo, Warehouse, Truck, Backlot and Mushroom Forest
  (cov11_hdrcam.py), plus the lift's girder objects (Stage2_MoveOnScrollingMap); not judged.
  (Rot3D_ProjectPoint's depth 0 is an Unreachable row now: the claw's and the cones' depth
  translations dwarf their points' z terms.) **Stage3_RoadCameraFollow** ($04062A, cov9's
  notes): Scr2_03E532 ("never run") is not a separate pattern but the straight continuation of
  pattern 2 Scr2_03E39A (no Goto between; $03E39A is entered by Op2_SetVector0HereGoto $03E07E
  in pattern 1), and it installs three osc segments with only stop segments between: $03ECBA
  ($03F412), $03ED1C ($03F384), $03EEE8 ($03F384), then a seek at $03EF64 (scratchpad
  cov11_steersites.py, cov11_waits.py: the timer waits along the span; Scr2_03E11A has no
  reference found). Three restarts from the current position could carry x past 384 (+-167 per
  run), depending on the phases. A probe from TAS f113300 with no input (scratchpad
  cov11_hatter.txt, not a corpus run) kept the boss alive until the game over at ~f119040: its
  pattern read script bytes up to $03EBB1 (the run's data map), short of $03ECBA; holding Left
  or Right loses the lives faster (5 lives in ~2900 frames). A scenario needs the player to
  survive longer without hurting the boss (two players, or steering round the road hazards).
  Scripts: scratchpad cov11_cov.py (which pcs a verify log covered), cov11_spans.py (a RAM
  word's changes over a run's dumps), cov11_minw.py (signed min/max), cov11_mirror.py (Batman's
  vs Robin's script words), cov11_opsites.py (op word sites in a range), cov11_runexec.py (an
  ad-hoc run's exec map at given pcs and its data map over a span: how far a script was read),
  cov11_corpdata.py (the same data map for a corpus run).
- **P4 D4 below $00A43C (cov10, 2026-10-28; whole-corpus verify on eec812f, kit 0.9.12, plus
  stepped-gen coverage: scratchpad cov3_d4.py with COV3_SG=1).** 18 files of the 37 were below
  $00A43C. Done by cases (2788ded): VM4_SpawnChild's nonzero failure offset (00613A.syn, at
  Op4_CounterKillOut: its 9 ROM uses all follow such a spawn), Op4_BranchCrossY's other side
  (0064BA.syn: a pool block's stale $58), Op5_StartGroup / VM5_CloseGroup with an open kill group
  (006F0A.syn, 006F56.syn), Pause_Update's debug jump (009C50.syn + row); rows for the group
  actions with no group ($006F74, $006F9E). The real pool does run
  out: VM4_SpawnChild's full-pool rts $006154 ran in the TAS and fuzz-73800-s2, and the TAS's
  g_ObjPoolFree falls to 10 at f73440 (scratchpad cov10_poolmin.py over the 60-frame RAM dumps).
  Left, with what is known:
  **Op5_StartGroup's full pool** ($006F4C-$006F54): reachable (ObjPool_Alloc on an empty pool),
  but it skips 4 of 6 operand bytes and jumps to the time-limit word as an op (`$0078` etc.: the
  vector table), so neither run can finish a case; it needs a ruling (a game bug a real state
  reaches, which no case can run). SpawnAction_GroupMemberReward / GroupMemberGone with d1 = 0
  ($006F74, $006F9E): Unreachable rows (every trigger table's action bytes walked: scratchpad
  cov10_trig.py). **Pause_Update's code completion** ($009CC6-$009CD0): run by scenario-level-skip's reference,
  never checked (the call waits in the pause loop) and not credited from stepped-gen (level-skip
  stops paused in both modes, so equal_coverage skips it). A one-skip scenario (level-skip's first
  section, then 600 idle frames) ran to the end on stepped-gen, but its stepped machine paused,
  read the presses (VPAD lines B A D B A D L U C) and never completed the code ($009CC6 not in its
  coverage; the pause interval's PADS line is the OR $F7 of every code button): not committed;
  why the frame-by-frame pause input doesn't match the code is open (a kit question).
  **VM6_SetupMapBlit's x clip** ($0074E2): likely dead, a game bug: it computes g_PlaneCellsX -
  width - d2, but d2 is the dest x in bytes plus the plane's VRAM address (every use passes
  g_PlaneAVRAM $EE34 or g_PlaneBVRAM $EE36: $C000 / $E000), so the 16-bit difference is about
  +$2000 / +$4000; a row needs every map's width (Op6_DrawMap/DrawMapVInt use the map's own width)
  bounded below $2000. **VIntObj_RunList** ($008AFE-$008B1E, 4+ V-int objects): the reference never
  had more than 3 (fuzz-30600-s2, the TAS: Stage2_RunLift's flush + lift scroll + a VM6 job; the
  truck: flush + Boss_JokerBalloon_StartSplit). Every VIntObj_New / NewIdle site is one-shot per
  scene (Scene_InitCommon's VIntObj_InitList empties the list) except the VM6 clear/blit jobs,
  freed only by their own end (the only VIntObj_UnlinkFree callers); the open point is whether a
  trigger's VM6_Run can start a second job while one is in flight (then 4 are possible).
  **The art refcount's positive byte** (Spr_SwapFrameArt $007FCA/$007FE6, Spr_ReleaseFrame
  $00803C, ArtCache_Acquire $0085F0-$0085F6, ArtCache_Release $0086D4-$0086E0): the byte is
  0 (cached) or minus the users; it goes positive only on a release at 0 (cleared at once) or 128
  users (no). Spr_InitBankFrames releases the art a handle still holds when a bank is loaded over
  it ($007ECA-$007EDC, never run: every bank in the corpus loads over handles the scene init
  cleared); that is the one designed way to release cached art (ArtCache_Release's positive path),
  if some scene loads two banks sharing handle offsets after art was acquired. Op4_DrawKillSidesBelowOrClear
  ($006854-$00691A) and Spr_ChangeFrame ($007F42, $007F5A) also have the "new and old frame share
  one handle" path (a ROM frame of Tbl_1DDB60 and a RAM frame with the same handle address): not
  checked against the data. **Op6_CallNativeRetry's park** ($0070C8, $0070D0): the natives that
  can return nonzero are Flight_WaitCloudsPassed and Stage3_SpawnRevealDirector (Obj_SpawnBoss:
  the TAS has 105 blocks free at its call, f97888); the clouds' $2400 loader starts from the VM5
  spawn entry $021D94 (`00A0 E070 0000 7092 0003 308A`: spawner $7092 with the segment), spawned
  by the `6FB2 0010` at $021D82 after `6ED2` (population 0) and `6AC4 12C0`: a population wait
  before it makes the start depend on how fast the player clears the wave, so the park may be
  real; which wave runs that script and Op5_Wait's units are not traced. **Scroll_FillBands'**
  band skip ($009934-$009938: the view y at or below the first band, tables like $9720 with a
  $60-line first band) and **SceneCb_WaitPlayersExit**'s no-player frame ($009DA8: neither player
  has an object while the exit timer runs) not judged. Scratch tools: cov10_spawnops.py,
  cov10_word.py (an op word's script uses), cov10_opargs.py (its operand words), cov10_exec.py,
  cov10_sg.py / cov10_sgrun.py (stepped-gen coverage), cov10_dump.py (a span's words named),
  cov10_poolmin.py.
- **P4 D4 at $020000 and above after cov7** (2026-10-28; list from the whole-corpus verify on
  7845923 + perf1's stepped-gen coverage, scratchpad cov3_d4.py with COV3_SG=1). Done by cases
  or rows: $036CD4, $03730E, $0268F2, $0471C0, $0477D4, $047A72, and $04062A's two-player paths.
  $03B91E: an Unreachable row (cov9). Left, with what cov9 found (2026-10-28):
  **$04062A** Stage3_DrawRoad's bend / pitch clamps ($0406D8-$04070E; bend = steerer x / 16
  beyond +-24 means |x| > 384 px, pitch = y / 16 needs y > 640 or < -7680). The road schedule
  only spawns hazards; the steerer (Scr2_03DA8C) is moved by its own loop (StepCoroutine1/0,
  StepVelocity, AddOsc $E, ApplyMove, Boss_Hatter_SteerRoad) and by coroutine-0 segments the boss
  installs with `2AA6 0012 off` (Op2_StartChildCoroutine0Rel on field $12, 40 sites in
  $03D108-$03F1DC, the only op addressing the steerer): 22 entries, decoded by scratchpad
  cov9_steer.py. Every target x is 0; target y in {-4500 (the exit, cruise 10 px/f), -4000, -400,
  -350, -250, -200, -150, 0, +150}. Segment kinds: seek (StopOsc, coroutine 1 = seek x then y to
  within 8 px, then target speeds 0), stop (coroutine 1 stopped, target speeds 0, the osc keeps
  running), osc (SetOscX/Y amplitude 32 / 64 / 96 px restarted from the current position, seek
  stopped: $03F2D2, $03F384, $03F404, $03F412). The 96 px X swing overshoots by ~71 px (max speed
  $1AA00, accel $500 with $2E(a4) = $100), so one osc run spans +-167 px around its start, and an
  osc segment never recentres: |x| > 384 needs osc segments installed with no seek between, which
  the boss's pattern scripts decide (Scr2_03E034 / 03E39A ran, 03E11A / 03E532 / 03EF80 never;
  Op2_SwitchRandom and damage vectors Op2_SetVector0HereGoto). Not walked. TAS: bend 0 the whole
  road (RAM dumps), pitch down to -276 only on the exit seek. **$02604C** the hull's -500 clamp:
  the tilt requests come from the two blast effects in Scr4_026A46 ($026B46 / $026C40: sound,
  flash, Shake_StartBig, Stage2_ArenaRequestTilt), refused while the floor turns; one full turn
  (slope +-$1800 to -+$1800, slope speed +2 a frame: 111 frames; roll +$2000 a frame to 3.0)
  lifts the hull ~224 px before its seek back (cruise $AA = 0.66 px/f, ~73 px over the turn);
  from y -40 ($025C66) it takes about 3 turns with short gaps: who spawns the blasts and how often
  is not traced. **$024660** the alt-terrain no-mask path ($02481E): a ROM scan of every
  terrain-box op's floor mask (scratchpad cov9_masks.py, $0-$60000) finds bit 0 clear only in
  `1E72 0000 FFxx 000x 0004` + Op2_DisableTerrainBoxB at $01AF8C (the brute's body) and the two
  player scripts $04ACFC / $04D838; both re-enable box B later (Op2_EnableTerrainBoxB $01B316,
  $04AD86, $04B576, $04D8C2, $04E0B2) and both use alt terrain. A player is a real arena mover:
  the question is whether any path enables box B (floor mask 4) while (a3) bit 0 (alt) is still
  set; $04AD84 (EnableA + EnableB on a fall, after Op2_BranchIfNotRising $04AD78) is the one to
  trace (scratchpad cov9_terr.py lists the terrain ops of a span in order). 9 arena runs (TAS,
  fuzz-30600..34200, continue, game-over) never executed it.
  Scratch tools: cov7_relcalls.py (calls to natives), cov7_relto.py (relative ops into a range),
  cov7_word.py (a word's ROM occurrences), cov7_depth.py, cov7_ladder.py, cov7_lab.py;
  cov9_words.py (script words named from build/symbols.txt), cov9_exec.py (runs executing a pc),
  cov9_ram.py (RAM from a run's 60-frame dumps), cov9_longs.py / cov9_relto.py (references).
- **P4 D4 at $020000 and above, what is left** (cov5, 2026-10-28; whole-corpus verify on 7845923,
  without stepped-gen coverage, which also covers the Story_* / Attract / SegaLogo / PlayerSelect
  scroll paths). Flight_RunScene $033274 waits on the user's ruling. Looked at, not done:
  Coll_Stage2TiltFloorRest's no-mask path $02481E-$024822 (d7 bit 0 clear with the alt terrain:
  which scripts give an alt-terrain mover such a mask is open); Boss_CatHead_FrameScroll $03B948
  (no raster set: Scene_InitStage clears g_SceneVars and every write in the arena is a routine,
  but that the curtain raster is installed before the first call, input-independent, is not
  proven: the driver's curtain coroutine $03B962 is started from elsewhere); Stage4_LabScroll
  $047822 (a wrap of LabFloorLines' d0). Not looked at: $02604C (the hull's -500 clamp: a
  synthetic case candidate), $0268F2 (the throw index clamps), $036CD4, $03730E, $04062A (14),
  $0471C0 (22: Stage4_ConeUpdate never runs), $047A72. Scratch tools that helped: a script-word
  dumper naming handler words from labels.toml, and stepping a routine's branch structure in
  Python over every value of its one input (Flight_DrawCorridor, CurtainRaster).
- **Answered (cov5, 2026-10-28: kit 0.9.10).** verify.py hands each verify process its member
  runs' reference exec maps (emu/coverage, instruction starts) in play order; verify.cpp probes a
  sampled-out call of an entry while its function file still has an instruction the reference
  executed in the run playing that no passing call or case covered (branches switch the targets at
  save / restore), and copies the post-call RAM only for a probe it compares. The movie, 0.9.9's
  runner and 0.9.10's at once: 587 s -> 615 s (+4.8%); 7 files gained coverage, among them all of
  $02417E's tilt paths, none lost any. Where the rest of the cost is: 6.2M probes, most in entries
  whose file has a path the run first executes late (VM4_Next 921k probes, its file's last op is
  first run late in the movie); activating each target at the frame its run first executes it
  would cut that, but the exec maps have no frames. Whole corpus on 7845923 (machine loaded by
  other workers): 982 s, 0 mismatches. Was:
  **P4 D4: verify never probes an entry's sampled-out calls** (cov3, 2026-10-28). verify.cpp
  compares every call until a function's 2000th, then 1 in 64; a sampled-out call is still a probe
  (compared if it reached an instruction not covered yet) only when DKIT_VERIFY_INSNS has the
  function's instruction count, and verify.py's insns_file writes one per functions.csv row (the
  files), none for the entries: `if (n == s_insns.end() || ...) return 0`. So a rare path in a hot
  entry is lost after its 2000th call: Stage2_LiftUpdate ($02410A, entry of $0240EE) arrives at
  the top once per ride (TAS f32065, after ~11,000 calls), the reference ran it in 3 runs, verify
  never. Tried: probing when the count is missing (`n != end && covered >= n`): whole corpus, all
  files, done 956 -> 977 but most of that is other commits' cases; the probe's own share ~7 files
  (02410A's arrival, Stage2_UpdateScroll's tilt paths $02417E, 005606, 003B76, 00799C, 008764,
  008DD2 among them); cost: the TAS alone 519 s -> 615 s (+18%; an entry whose range [entry, file
  end) never completes probes for ever). Not shipped: a kit decision (S3's 15-min gate). Cheaper
  ideas: per-entry counts over the instructions the reference ran from that entry; or a probe
  budget per entry. Meanwhile 02410A.syn pins the arrival.
- **P4 D4 below $020000 (cov4, 2026-10-28, whole corpus on 33ce4f8 + its cases).** Files < $020000:
  done 515 -> 538, uncovered 91 -> 68, never called 11, every call waits for the V-int 6. The
  sprite V-flip (bit 4 of $38(a5)) is never set: one row lists every writer (Op2_BranchPlayerInBox
  $003434); scans: scratchpad cov4_ops.py (op words + operands per span), cov4_mirror.py (users of
  a draw op and whether their script mirrors), cov4_scenehdr.py (g_SceneSprAttr of every scene
  header). Left, with what is known: `Op6_CallNativeRetry` $0070BE parks for real
  (Flight_WaitCloudsPassed returns $F597 = $FF right after Flight_StartClouds) but has 0 checked
  calls (the flight scenes' calls all wait or are skipped): needs `at $ADDR` with a stage 2 state;
  `VM4_SpawnChild` $00614E: the failure branch needs a spawn op with a nonzero failure offset (e.g.
  $014E4A `5606 0006 ...`) when the pool is full, and the first call's op may have 0;
  `Op2_AnimHoldByVelY` $003F0C (vy + ay + bias < 0) and `Spr_ReleaseFrame` $00803C (a refcount
  above 0) not yet judged real or not; the HUD two-player paths ($011100-$011314),
  Player_GetP2Var1 and Camera_TrackPlayersX want player 2 in play with a live object
  (scenario-two-player joins P2 but these callers don't run there).
- **P4 D4 below $020000 (cov6, 2026-10-28, whole corpus on 9c4d9c2 + kit 0.9.10 + its cases).**
  Files < $020000: done 539 -> 578, never called 8 -> 0, uncovered 68 -> 37, the 6 V-int-wait
  files left (the queued question). Boot_CheckRegion, Boot_InitIOPorts and Main_Start are tagged
  noreturn (they never return: D2/D6 verify them). New corpus run scenario-two-player-exit (P2
  joins after the Harley mid-boss, both exit, both go out, pad 2 continues). Dead code given its
  checked call by a synthetic case, as 015730.syn's debug code: the debug pause menu (01569C,
  0156FC, 015716), Op2_ClearMotionBlock (no script word), Obj_NewMulti_ClearRun (every caller
  passes 4+ blocks). Found: Op5_StartGroup on a full pool skips 4 of its 6 operand bytes and
  jumps to the time-limit word as an op (a game bug; no case: both runs execute data). Left, with
  leads: `Op6_CallNativeRetry`'s park $0070C8: the reference never parks (the TAS runs
  Flight_WaitCloudsPassed once and gets 0); its $2400 loader $03308A is started by the trigger
  at $021D94 in Tbl_021D3A, so the question is whether that trigger can fire before
  Flight_FrameClouds clears g_SceneVars+$A3 at layer x $E40 (an auto-scroller: likely never, not
  proven); `VM4_SpawnChild`'s failure branch needs a state where a spawn op with failure offset 6
  ($014E4A, $01A40A, $01F22A, $020E7A...) runs on a full pool, and pointing a4 at another
  object's operands would run that script on the wrong object; `Layer_MoveX` (positive moves)
  and `Layer_MoveY` (negative): callers pass constants and Op6_MoveLayerAToCamera's camera +
  ($FEAC, $002C); unproven either way; the Spr_/ArtCache_ frame and cache files, VIntObj_RunList
  (more than 3 V-int objects), Coll_MoveBlockMap / Coll_WalkBlockMap, Layer_QueueColumn,
  Rot3D_ProjectPoint: not judged. Scripts: scratchpad cov6_mark.py (a file's asm with the
  uncovered marked), cov6_hits.py (which uncovered instructions a run's exec map has), cov6_mk2p.py
  (scenario lines from the movie's P1 inputs plus a P2 overlay).
- **P4 D4 below $020000 (cov8, 2026-10-28, no whole-corpus verify: each case verified on the
  runs that reach its `at`).** Done: the dead-code rows of the queued question's default (a)
  (debug pause menu, free camera + tails + hex readouts, Frame_RunFreeCam's jmp,
  Op2_ClearMotionBlock, Obj_NewMulti_ClearRun); rows for Item_WeaponItemGone's floor,
  Camera_TrackPlayersX's clamp, Rot3D_TransformPoint's z term; cases for a full art cache
  (011D04 / 0085B0: no block, no descriptor; 0086AA: BossMeter_Free of an empty handle). Every
  uncovered instruction of the 37 files but Pause_Update's, Section_RunNext's and
  Stage1_RunTruck's (the V-int waits) is one no committed corpus run executes. Verify trap:
  `recomp verify --functions FILE` tests that address only, not the file's entries (pass them
  too: scratchpad cov8_ents.py prints the list); a case's callee gets no coverage from it (a
  case on BossMeter_Init left ArtCache_Acquire uncovered: it needed its own `at $011D04` case).
  Leads: the art refcount paths (Spr_SwapFrameArt $007FCA/$007FE6, Spr_ReleaseFrame $00803C,
  ArtCache_Acquire $0085F0, ArtCache_Release $0086D4-$0086E0) are a positive refcount byte (a
  release of unused art) or a second user of the boss meter's art: BossMeter_Show with a live
  meter ($011ADC: it restarts Scr2_011B3E, so Init acquires again) would give the latter, if a
  boss's Show can come within the old meter's exit (Scr2_011B8E, Op2_SetTimer1 $780) or a boss
  calls Show twice; Spr_SwapFrameArt $007FEA (no descriptor; scenes have up to 60 blocks + $D, so
  50 descriptors can run out) has its case (007F5E.syn);
  Weapon_KeepShotArtOne $0109E6 = two weapon changes within 20 frames (a scenario could pick up
  two weapon items quickly); Span_BuildCircle / Span_DrawDiscDither's neg = g_BeamRadius < 0
  (only the radius object's x, Scr2_029598 / Scr2_0295D4); Layer_MoveX's positive move needs
  g_CameraX > $154 at Op6_MoveLayerAToCamera ($330BE, $331E0); Layer_MoveY's negative move needs
  g_CameraY < -$2C there (its other callers pass $1910, $8, $6E8); Boss_Harley_StepPole's $82 cap ($01A1EC) depends on
  the pole targets of Boss_Harley_SetPoleTarget's operands and the bounce; Rot3D_ProjectPoint
  $01543A (depth word 0) not judged.
- **Answered (cov3, 2026-10-28: kit 0.9.8, verify's MAX_DEPTH 64; a call refused for depth now
  counts as the function's `too_deep`).** Whole corpus on be9f311, all files: MAX_DEPTH 16 -> 40
  done 909 -> 943, no checked call 35 -> 12, uncovered 158 -> 147; 40 and 64 the same file by
  file. Proof: Title_MenuInit $0499B6, 0 checked whole-corpus at 16, 1 alone, 1 with every other
  function but Title_Run (or but Scene_LoopUnskippable). Was:
  **P4 D4: the whole-corpus verify leaves code untested that it tests alone** (cov2, 2026-10-28).
  25 files have instructions the reference ran (emu/coverage exec bitmaps) that verify never
  covered. Example `Op2_LaunchAlongHeading` $0014F0: 0 checked calls in the whole-corpus verify,
  but 22 on fuzz-36000-s1 alone with `--functions 0014F0` (all deferred, all 17 instructions
  covered). It is still checked with its file's entries and VM2_RunObject also under test, and
  gets 0 again once the first half of all functions ($0003FA-$00AEA4) is added. Cause (cov3):
  verify.cpp's MAX_DEPTH = 16 refuses calls nested 16 tested contexts deep, and the scene
  routines are at the bottom of every nest; kit 0.9.8 raises the limit. Measure these files again
  after that before giving them cases or table rows: 0014F0 00156A 0017EA 001C3C 004898
  005606 005D74 007416 007EF6 009C50 009D24 009FBC 00AC66 00B1A4 010742 010E0A 011314 011B06
  012A84 014440 0144B2 014F78 01A04C 01E9A4 01FECC (stepped-gen coverage, kit 0.9.7, has
  already covered some of them). The never-called files below $020000 (Boot_*, TextMap_*,
  Item_*) are probably the same case.
- Code-as-data at the tail of the hit callback Enemy_VentTank_PuffHit (the R5 entry): the coverage's
  reader records are per function and span, so they can't say which VM op reads those 4 bytes.
  Next step: a reference run of the TAS with a per-byte reader watch over that range.
- 372 a2 NEXT sites never ran: handlers no run reaches stay raw; scenarios may reach some.
- **Answered (cov7, 2026-10-28): it does run.** The whole-corpus verify on 7845923 checked 19,029
  calls of Stage4_ConeUpdate (60 instructions covered); what it left (the escape path's closing
  cone, the one-turn sweep) has 0473BA.syn and an Unreachable row. Was:
  `Stage4_ConeUpdate` ($0473BA) never executes in the corpus (verify: 0 calls and no covered
  instruction on tas, scenario-level-skip and the 16 stage-4 fuzz runs; w27, 2026-10-28): its only
  callers are the Op2_CallNativeRel words at $0421EC / $042222 in gas-orb loop scripts, and no orb
  in the corpus runs them, though the header's TAS RAM shows cone widths easing in the hall. Its C
  (0471C0.c) is untested: ruling 5's queue (a scenario or fuzz seed with a cone-owning gas orb).
- **Answered (inv1, 2026-10-28): a call from C unwound at an RTS inside its callee that no call
  site waits for.** w29's scene routines (`jsr VM6_Run`) and w15's trigger dispatch (Trigger_Fire's
  `jsr (a0)`) in C stopped the stepped TAS ("entry point returned unexpectedly", sync 0 / 352 /
  2,762). Not the V-int or the LZ task: `DK_UNWIND_LOG=1` (handc_rt.cpp) printed
  `call of $0070A4 from C $049DA6: rte_pending 0, rts to $0070B0, A7 $FFFFFFF0 (call's $FFFFFFF4)`:
  the script's Op6_LoadPalettes does `pea VM6_Next(pc)` and its helper's rts lands at VM6_Next,
  where the 68000 runs on (Stage1_BlastSignChained's `move.l (a1),-(a7) / rts` is the same shape).
  The C's call waited for ANY return (`XAWAIT_ANY`), took that rts as VM6_Run's own and threw
  dk::Unwind. Fix, kit 0.9.2: `CALL_FROM_C` waits for RET_FROM_C (as a generated JSR waits for its
  own return address) and verify's runs for the function's own return address (it compared
  Trigger_Fire mid-callback: RAM $FFFFD5, the live return slot). Second cause, the C pattern:
  VM6_Run's measured contract ends at its `jmp (a0)` (regs(&a6)), so the stub restored the
  registers the script left for the spawners (D2 sync 1, ram_crc32); its prototype now passes
  every register (decomp-function's shape table). $049AA6, $049DA6, $012E02, $012E36, $012E5C are
  C (gate: D2 6,000 syncs equal, D4 0 mismatches); the other `X_Run` files are open to C.
- **Answered (inv2, 2026-10-28): Boot_Reset is C (000300.c)**: kit 0.9.5's `dk::set_usp` and
  `dk::set_status_register`, the RAM clear through g_SceneBuf, the init table read as the asm's
  (a5)+, and a prototype passing every register in/out (the movem load handed on by the
  `dk::jump` to Boot_CheckRegion); D2 6,000 syncs equal from power-on. Was: **P4: Boot_Reset
  ($000300) has no C** (w01, 2026-10-28). Out of the bridge's shapes: `move.l a6,usp`
  (no USP API), the 64 KB RAM clear through `move.l d0,-(a6)` from address 0 down (work RAM by
  address: neither globals nor platform.h name the whole of it), and `movem.l (a6),d0-d7/a0-a6`
  loading every register from the cleared RAM before `bra Boot_CheckRegion`, while the
  prototype (`noreturn`, inputs only) has no outputs to hand them on. Needs a platform/bridge
  decision (or ruling 6's vector exception) first. Boot_CheckRegion's lockout path and
  Boot_DrawLockoutText (C since w01) only run on a console whose region is not J or U: no corpus
  run has one; a scenario with the emulator's region set to Europe would reach them (ruling 5).
- **Answered (inv2, 2026-10-28: kit 0.9.5's verify masks [stack_low, $10000) when the returned A7
  is 0; 011050.c, D4 0 mismatches).** Was: **P4: Player_InitRecords ($011050) has no C** (w13, 2026-10-28): verify's dead-stack bound wraps
  at the top of the stack. The title flow calls it at depth 1 (SSP vector 0, so its return slot is
  $FFFFFFFC and the returned A7 is $00000000); verify.cpp takes `hi = A7 & $FFFF` = 0 < lo, empties
  the dead-stack range, and then compares the slot its `bsr Player_InitRecord` pushed below:
  `tas: call 1: RAM FFFFF9: generated 01 (the bsr's $0001109A), C F0 (RET_FROM_C $F00001)`. The C
  (both calls, the second a `dk::jump` with a0 widened to an output) was written but not committed;
  it needs verify to treat A7 = 0 as $10000 (a kit fix), then it goes in as is.
- **Answered (w10, 2026-10-28): a pointer-typed parameter fails on a register that holds no memory
  address.** The bridge converts a pointer parameter's register with `dk::host` (dk_bridge.h
  `from_u32`), which is fatal for anything but work RAM or ROM. Frame_End in C stopped the stepped
  TAS at sync 14,160: `dk::host: $00C00000` (a0 = the VDP data port, left by the SAT DMA code, on
  its way into Pause_Update's in/out a0). The 6,000-sync gate never got there. Rule: an in/out
  register the function only writes (or whose input may be a port, a count, garbage) stays
  `uint32_t *` and the C writes `dk::addr(p)`; type a pointer only when every caller's input is
  one (Pause_Update, Pad_ReadLive, Player_GetPnVar1's a4, the parallax chain's a1/a6 retyped so).
  Also: VM6_RunHook's measured contract ends at its `jmp (a0)` into the a6 script, like VM6_Run's;
  with regs(&d0,&a0) Frame_End's C diverged at sync 4,386 (ram_crc32), with every register passed
  it is equal (stepped vs stepped-gen, the TAS's first 40,000 syncs: same sync_sha1).
- **P4: Debug_FreeCamStep ($015730) has no C** (w16, 2026-10-28). Static only (g_FreeCam has no
  writer outside the never-run Debug_PauseMenu), so neither D2 nor verify could check it. Blocked
  on: seven RAM words it uses by number ($F426/$F428 the cursor's block x/y, $F42A a hex readout
  temp, $F42C/$F430 the map position longs, $F434 the cursor's sprite frame) that need `disasm
  global` names first, and its `bsr Coll_MoveScene` with d1/d2/d4-d7 set and d4/d5 read after,
  while Coll_MoveScene's measured contract is regs(a3)->a0 (the corpus never ran this call site):
  the prototype would have to be widened on reading alone. It ends in a bpl / bra into
  Debug_FreeCamScroll ($015916-, the next worker's range).
- **P4 (w17, 2026-10-28): the free camera's tail is C but never checked.** Debug_FreeCamScroll,
  Debug_FreeCamSyncLayer (Debug_FreeCamSyncLayer, named: Debug_FreeCamStep's bra target) and
  Debug_DrawHexBytes have C (015916.c, 015940.c) from the asm alone: no corpus run enters the free
  camera, so verify has 0 calls and D2 never runs them. They need ruling 5's synthetic cases
  (`recomp/game/verify/015916.syn`, `015940.syn`: g_FreeCam set, a cursor right / left of centre,
  a 2-byte readout) or "Unreachable instructions" rows with Debug_FreeCamStep's reason.
- **P4 (w17): Boss_Claw_DrawProjected ($01EB9E) is an entry of 01EB86's file** (named from
  Boss_Claw_DrawProjected: Boss_Claw_DrawChain branches into Boss_Claw_DrawPoints' loop after its own
  projection, its count pushed for the loop's `move.w (a7)+,d7`). The C for 01EB86 must define
  it. Also learned there: a `movem.l a2-a6` saved around calls does not stop the callees in
  between from seeing the registers the first call changed: Boss_Claw_Draw passes
  Rot3D_BuildMatrix's a2 on to DrawChain/DrawPoints (with the entry a2, verify: d2 differed at
  the first call).
- **P4 (w16): `recomp entries` does not list V-int list callbacks**: Layer_FlushTileQueueIfIdle
  ($012CBE, registered with `lea ...,a0 / jsr $8B7E`) sits in Layer_QueueColumn's file, but the
  entries listing of $012A84 named only $012A84/$012B74; the stepped runner's fatal (`[handc] entry
  $012CBE ... has no C`) found it. Read a file's asm for every label, not only the listing.
- **Answered (inv2, 2026-10-28: kit 0.9.5).** (1) each call from C has its own return address
  (RET_FROM_C + 2 x depth), so a return onto an outer C call's slot unwinds the C frames in
  between: 009FBC.c / 009FE6.c in, the whole TAS stepped vs stepped-gen 130,055 syncs equal (the
  control with one address for all: sync 141). (2) verify's dead stack at A7 = 0: 00A1BE.c in, D4
  0 mismatches. (3) `dk::status_register()`: 00A1F0.c in. Was: **P4 (w10, 2026-10-28): three files
  of $0096B6-$00A43C wait on the harness.** (1) `Scene_RunList`
  and `Section_RunNext` in C (parked, the C written) stop the stepped TAS at sync 141: `[handc] call of
  $009D54 from C came back with A7 $FFFFFFF8 (expected $FFFFFFF0) (C function $009BF0)`.
  SceneCb_WaitFadeExit returns past Frame_End and the scene loop (`addq.l #8,a7 / rts`) into the
  scene routine's caller; with that caller in C too, the RTS lands on RET_FROM_C of the outer call
  (Scene_RunList's SceneFn), which the inner call (Frame_End's FrameCallbackFn) takes as its own and
  refuses. The bridge would need to unwind to the C frame whose A7 matches. (2) `Sound_PlayMusic`
  (+ AnyWay / RestartMusic / MusicDone; C parked) fails verify only on its first call (tas: RAM
  $FFFFE1, generated $00, C $F0): Main_Start calls it with A7 = $FFFFFFFC, the returned A7 is
  $00000000, and verify.cpp's dead-stack mask (`hi = A7 & 0xFFFF` = 0 < stack_low, so hi = lo)
  masks nothing; the bytes are the return-address slot of its `jsr Snd_StopChannels` (the jsr's
  $0000A1D6 against the bridge's RET_FROM_C). Every other call is equal. (3) `BgTask_Start` saves
  `move.w sr,d0` into the task context: the SR's system byte is live (BgTask_Resume's RTE restores
  it) and no dk:: call reads the SR (only set_interrupt_mask): a getter first.
- **Answered (inv2, 2026-10-28: kit 0.9.5, 009E02.c in, whole TAS equal).** Was: **P4 (w11,
  2026-10-28): Main_Start ($009E02) waits on the same harness limit.** Its C (written,
  parked outside the tree: one function, the labels as gotos, `push_word` / `set_stack_pointer`
  around Continue_Run so the stack depth matches) stops the stepped TAS at sync 352: `[handc] call
  of $009D54 from C came back with A7 $00000000 (expected $FFFFFFF8) (C function $009BF0)`. The
  title's exit fade (SceneCb_WaitFadeExit, `addq.l #8,a7 / rts`) returns past Frame_End and the
  title loop into Main_Start's call of Title_Run: the case (1) above with Main_Start as the outer C
  caller. Everything else in w11's range is C (D2 equal on the whole TAS, 130,055 syncs).
- **Answered (inv2, 2026-10-28: kit 0.9.5; `gate --sync 135000` with 002CA4.c in: 130,055 syncs
  equal).** Was: **P4 (w11, 2026-10-28): the whole-TAS D2 stops at sync 10,099 on main** (seen at 7c967b0):
  `[handc] call of $0102C8 from C came back with A7 $FFFFFFEC (expected $FFFFFFE4) (C function
  $002CBC)`: Op2_CallNative's C (002CA4.c) calling Player_DespawnNative, which returns past its
  caller (8 bytes). The gate's 6,000-sync cut never reaches it; with 002CA4.c taken out locally the
  TAS is equal to the end. The VDP clears in C also showed a verify trap worth knowing: a bridge
  call where the asm has a `bra` pushes a return address the asm doesn't; at Main_Start (A7 at the
  top of RAM) verify compares that slot (VDP_ClearCRAM call 1: $FFFFF9 generated 00, C F0), so
  those files inline their fill loop.
- **Answered (w08, 2026-10-28: both lines declared in f744f82, 00B232.c and 00EAA2.c are loops;
  $00F7F8 is named Span_FillRampsDither0Odd).** **P4: two unrolled runs have no `computed_entry_runs` line, so their files can't be C yet** (w12,
  2026-10-28). `00B232` (Mem_FillRampRun + Mem_FillRamp + Span_FillRampsRun + Span_FillRamps +
  Mem_FillWordsRun) and `00EAA2` (Span_FillRampsDither1Run + Span_FillRampsDither1 + Span_FillRampsDither0Odd):
  `recomp entries` lists every unit the reference's computed jumps reached as a required entry
  (222 of Span_FillRampsRun, 151 of Span_FillRampsDither1Run), because only declared runs drop
  their interior once the `from` file is C. The lines they need (dkit.toml, the user's or the
  orchestrator's to add): `{ start = 0x00B93C, end = 0x00C73C, stride = 16, from = 0x00C742 }`
  (Span_FillRamps' jmp) and `{ start = 0x00EAA2, end = 0x00F7DA, stride = 18, from = [0x00F7F4,
  0x00F804] }` (Span_FillRampsDither1's even jmp and Span_FillRampsDither0Odd's odd one; units $E81A-$EAA1
  before the run are the same pattern, never reached). With them, both files are loops like
  00C908.c / 00DAB4.c. Mem_FillRampRun's run ($B232) and Mem_FillWordsRun's ($C746, from
  00C908.c, C now) are declared already.
- **P4 (w26, 2026-10-28): verify can hardly check the Hatter road** ($03D4B8-$040C85, all C).
  The road's H-int fires every line (scroll mode $28), and verify skips every call an interrupt
  lands in (`irq_skipped`): over fuzz-111600/113400/115200 (s1+s2) + the TAS, Stage3_DrawRoad had
  5 checked calls against 16,100 skipped, and the spawners, Fx_RunScript launchers, road-schedule
  commands and the hazard's init/hit have 0 checked. Their only full check is the whole-TAS D2
  (TAS f113271-f116145), which waits on the 002CA4.c fix above. The six stage 3 section routines
  never return (0 checked anywhere): D2 too. Checked locally at ba30679 with 002CA4.c taken out
  (as w11 did): `gate --sync 135000` D2 equal at all 130,055 sync points, so stage 3 and the gas
  orbs in C match the generated code over the whole TAS. Worth a kit question: verify with the H-int held off
  (as the synthetic cases do) for functions whose RAM effects don't depend on it.
- **P4 (w26): the road schedule's never-run commands stay raw.** Boss_Hatter_RoadCmdNext jumps
  into Boss_Hatter_RoadCmdTbl by the command nibble; only entries 0 (end), 2 (goto) and 8 (spawn
  kind 0) are code. Commands 1 wait ($03DCDC), 5 set loop ($03DCE6), 6 loop ($03DCEC), 3 gosub
  ($03DCF4), 4 return ($03DD0E), 7 random ($03DD1E) and the spawn stubs of kinds 1-7
  ($03DD5A-$03DD8F, spawners $3FB24 ...) are incrom (read with `disasm peek`), so a schedule using
  them would stop the generated code and the C alike. The loop count is a byte at g_SceneVars+6.
  Superseded 2026-10-30 (raw-stubs): the schedules the boss starts do use them (`0503` at $03F60C,
  `0702` at $03F920, kinds 1-7 throughout $03F5D8-$03FAB0). All of them are code with hand C now,
  and HatterRoadScene.loop is the u8 the code uses.
- **P4 (w18, 2026-10-28): a call into an a4 script under-reports its contract.** Fx_RunScriptNoParent's
  measured regs(&a0,&a4,&a6) ends at `bra VM4_Next`; the script reads d2-d5 and changes any register
  (verify of unk_Stage1_01FF78: a1 generated $FFDE94, C the entry value; Stage2_UpdateScroll: a5).
  Its prototype now passes every register, and a C caller widens its own prototype to the registers
  the script leaves (the measured contract listed them as inputs only). Also seen in $01EB86-$024575:
  a branch into another file's interior (`bne.s $245B0` in Stage2_ArenaVIntScroll) is a dk::jump to
  the entry its file's C named there (Stage2_ArenaVIntColumns), and a loop count kept on the 68000 stack
  (`move.w d7,-(a7)` ... `move.w (a7)+,d7`, no pop_word in dk_flow.h) is read through
  `dk::stack_pointer()` / `set_stack_pointer(sp + 2)` (Boss_Claw_DrawProjected, Stage1_SignDebrisRun).
- **Answered (inv2, 2026-10-28: kit 0.9.5's `dk::usp()` / `set_usp`; 007F5E.c in, Spr_ChangeFrame's
  C parks a1 in usp and jumps into it as the asm's bne; D4 0 mismatches).** Was: **P4:
  Spr_SwapFrameArt ($007F5E) has no C** (w08, 2026-10-28): every exit ends `move.l usp,a1`,
  handing back the a1 that Spr_ChangeFrame parked in usp (`move.l a1,usp`) before its bne into it.
  The C can't read usp (no USP API, as Boot_Reset), and verify compares a1 on every call: with a1
  as an input-only prototype parameter the gate's d4 failed at the first TAS call (`a 000001:
  generated FFFFB024, C 001DDB60`). Spr_ChangeFrame's C calls the generated Spr_SwapFrameArt (not
  the bne) and returns the caller's a1 itself; nothing else reads that usp (every other usp use
  is a save + restore in one function). A C version (the resident / allocate / fail / release
  paths, ArtCache_Alloc + Spr_QueueArtDMA calls) was written and matched everything but a1; it
  needs a `dk::usp()` read (kit) or a ruling that a1 there is Spr_ChangeFrame's, not its own.
- **P4 (w14, 2026-10-28): HUD_StepIdle ($011310) is C but never checked.** Slot 0 of the HUD step
  table runs only when g_HUDStep is 0 at HUD_Update: before HUD_Init, or after HUD_StepLoadArt's
  DMA_Reserve failed (HUD_Update clears the step before the jump and LoadArt sets 8 only on
  success, so a full queue there stops the HUD's DMA cycle for good: the original's). No corpus
  run executes $011310 (exec bitmaps, 153 runs). Its C is an empty function (the rts); it needs
  ruling 5's synthetic case or an "Unreachable instructions" row with that reason.
- **Answered (inv2, 2026-10-28: kit 0.9.5's verify mask; 1E500E.c in, D4 0 mismatches).** Was:
  **P4 (w30, 2026-10-28): Snd_ResetFade ($1E500E) has no C: the dead-stack mask at boot.** Its C
  (`return Snd_ResetFadeBody(&g_SndFadeLevel);`) is equal on 18 of 19 TAS calls; call 1 is the
  boot one, entered by Snd_Init's `bra.w`, so its return pops Main_Start's slot and leaves A7 =
  $00000000, and verify's dead-stack mask (`hi = A7 & 0xFFFF` = 0) masks nothing, as for
  Sound_PlayMusic (w10, above). The first byte compared is $FFFFED, the a1 that Snd_WriteFadeByte's
  `movem.l a1,-(a7)` saved (generated: the stub's `lea Snd_ResetFadeBody,a1`, $1E799A; C: the
  caller's a1); below it sit the stub's own a1 save and its `jsr (a1)` return slot, which the
  bridge's return address can never match. Every other file of $04A64C-$200000 is C (the driver
  bodies, Snd_Init's `bra` into the generated Snd_ResetFade). Needs the verify mask to treat an
  A7 that wrapped to 0 as $10000 (kit), then the one-line C.
